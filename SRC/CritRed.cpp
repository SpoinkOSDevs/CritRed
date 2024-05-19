#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/resource.h>
#include <unistd.h>
#include <csignal>
#include <filesystem>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

// Configuration
const string LOGS_DIRECTORY = "/var/log/"; // Directory containing log files
const int REBOOT_DELAY_SECONDS = 20;  // Default delay before rebooting after a panic

// Function to display the red screen with critical error message
void show_crit_error(const string& debug_info) {
    cout << "\033[1;31m"; // Set color to red
    cout << "----------------------------------------" << endl;
    cout << "|              CRITICAL ERROR           |" << endl;
    cout << "----------------------------------------" << endl;
    cout << "|     Kernel Panic - System Failure    |" << endl;
    cout << "----------------------------------------" << endl;
    cout << "| Debug Information:                    |" << endl;
    cout << "| " << debug_info << endl;
    cout << "----------------------------------------" << endl;
    cout << "\033[0m"; // Reset color
}

// Function to check for kernel panic in log files
void check_log_files() {
    for (const auto& entry : fs::directory_iterator(LOGS_DIRECTORY)) {
        if (entry.path().extension() == ".log") {
            ifstream log_file(entry.path());
            string line;
            string debug_info;
            while (getline(log_file, line)) {
                if (line.find("Kernel panic") != string::npos) {
                    debug_info += line + "\n";
                }
            }
            if (!debug_info.empty()) {
                show_crit_error(debug_info);
                // Reboot after delay
                cout << "Rebooting in " << REBOOT_DELAY_SECONDS << " seconds..." << endl;
                this_thread::sleep_for(chrono::seconds(REBOOT_DELAY_SECONDS));
                system("reboot");
            }
        }
    }
}

// Function to handle termination signals
void signal_handler(int signum) {
    cout << "Received signal " << signum << ". Exiting..." << endl;
    exit(signum);
}

// Function to install the program and create a service
void install_program() {
    // Copy executable to /bin directory
    system("sudo cp CritRed /bin/CritRed");
    // Create service file
    ofstream service_file("/etc/systemd/system/critred.service");
    service_file << "[Unit]" << endl;
    service_file << "Description=CritRed - Linux Kernel Panic Monitor" << endl;
    service_file << "After=network.target" << endl;
    service_file << endl;
    service_file << "[Service]" << endl;
    service_file << "Type=simple" << endl;
    service_file << "ExecStart=/bin/CritRed" << endl;
    service_file << "Restart=always" << endl;
    service_file << endl;
    service_file << "[Install]" << endl;
    service_file << "WantedBy=multi-user.target" << endl;
    service_file.close();
    // Enable and start the service
    system("sudo systemctl daemon-reload");
    system("sudo systemctl enable critred.service");
    system("sudo systemctl start critred.service");
}

int main(int argc, char* argv[]) {
    // Check for installation option
    if (argc > 1 && string(argv[1]) == "--INST") {
        cout << "Installing CritRed..." << endl;
        install_program();
        cout << "CritRed installed successfully." << endl;
        return 0;
    }

    // Check for Crash Testing mode
    if (argc > 1 && string(argv[1]) == "--DT") {
        cout << "Crash Testing mode activated." << endl;
        show_crit_error("This is a test of the emergency panic system.");
        return 0;
    }

    // Set higher priority for the process
    setpriority(PRIO_PROCESS, getpid(), -10);

    // Register signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    cout << "CritRed - Linux Kernel Panic Monitor" << endl;
    cout << "-------------------------------------" << endl;

    // Start monitoring log files for kernel panics
    cout << "Monitoring log files for critical errors..." << endl;
    check_log_files();

    return 0;
}
