#include <iostream>     // For input and output
#include <fstream>      // For file input/output operations
#include <cstdlib>      // For system calls (e.g., for copying files)
#include <filesystem>   // For filesystem operations (available in C++17)
#include <string>       // For string manipulation

bool install_program() {
  std::cout << "Installing CritRed as a service..." << std::endl;

  const std::string service_file_path = "/etc/systemd/system/critred.service";

  // Copy executable with appropriate permissions (adjust as needed)
  if (system("sudo cp CritRed /usr/local/bin/CritRed -p u=root,g=root") != 0) {
    std::cerr << "Error: Failed to copy CritRed executable." << std::endl;
    return false;
  }

  // Write service file content
  std::ofstream service_file(service_file_path);
  if (!service_file.is_open()) {
    std::cerr << "Error: Unable to open service file for writing." << std::endl;
    return false;
  }

  service_file << "[Unit]\n";
  service_file << "Description=CritRed - Linux Kernel Panic Monitor\n";
  service_file << "After=network.target\n";
  service_file << "\n";

  service_file << "[Service]\n";
  service_file << "Type=simple\n";
  service_file << "ExecStart=/usr/local/bin/CritRed\n";
  service_file << "Restart=always\n";
  service_file << "\n";

  service_file << "[Install]\n";
  service_file << "WantedBy=multi-user.target\n";

  service_file.close();

  // Use systemd calls if available (replace with systemd library calls if possible)
  if (system("sudo systemctl daemon-reload") != 0) {
    std::cerr << "Error: Failed to reload systemd daemon." << std::endl;
    return false;
  }

  if (system("sudo systemctl enable critred.service") != 0) {
    std::cerr << "Error: Failed to enable CritRed service." << std::endl;
    return false;
  }

  if (system("sudo systemctl start critred.service") != 0) {
    std::cerr << "Error: Failed to start CritRed service." << std::endl;
    return false;
  }

  std::cout << "CritRed installed successfully." << std::endl;
  return true;
}
