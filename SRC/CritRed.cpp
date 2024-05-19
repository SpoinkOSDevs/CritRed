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

// Function to display usage instructions
void display_usage() {
    cout << "Usage: CritRed [OPTION]" << endl;
    cout << "Monitor Linux kernel logs for critical errors and react accordingly." << endl;
    cout << "Options:" << endl;
    cout << "  --help, -h     Display this help message and exit" << endl;
    cout << "  --DT           Activate Crash Testing mode (simulate kernel panic)" << endl;
    cout << "  --INST         Install CritRed as a service" << endl;
}

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

// Function to install the program as a service
void install_program() {
    cout << "Installing CritRed as a service..." << endl;
    // Copy executable to /bin directory
    system("sudo cp CritRed /bin/CritRed");
    // Create service file
    system("sudo touch /etc/systemd/system/critred.service");
    // Write content to service file
    ofstream service_file("/etc/systemd/system/critred.service", ios::app);
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
    cout << "CritRed installed successfully." << endl;
}

int main(int argc, char* argv[]) {
    // Check for options
    if (argc > 1) {
        string option = argv[1];
        if (option == "--help" || option == "-h") {
            display_usage();
            return 0;
        } else if (option == "--DT") {
            cout << "Crash Testing mode activated." << endl;
            show_crit_error("This is a test of the emergency panic system.");
            return 0;
        } else if (option == "--INST") {
            install_program();
            return 0;
        } else {
            cout << "Unknown option: " << option << endl;
            display_usage();
            return 1;
        }
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
