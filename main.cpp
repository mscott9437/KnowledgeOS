
#include <iostream>
#include <map>
#include <sstream>
#include <string>

struct RuntimeState {
  std::string name = "KnowledgeOS";
  std::string version = "0.2.0";

  bool running = true;

  unsigned int commands_processed = 0;
};

struct CommandRegistry;

using CommandHandler = void (*)(RuntimeState &, const CommandRegistry &);

struct Command {
  std::string description;
  CommandHandler handler;
};

struct CommandRegistry {
  std::map<std::string, Command> entries;
};

void register_command(CommandRegistry &registry, const std::string &name,
                      const std::string &description, CommandHandler handler) {
  registry.entries[name] = {description, handler};
}

void display_help(const CommandRegistry &registry);

void help_handler(RuntimeState &state, const CommandRegistry &registry) {
  (void)state;
  display_help(registry);
}

void status_handler(RuntimeState &state, const CommandRegistry &registry) {
  (void)registry;

  std::cout << std::endl;
  std::cout << "KnowledgeOS Runtime Status" << std::endl;
  std::cout << "--------------------------" << std::endl;

  std::cout << "Name: " << state.name << std::endl;
  std::cout << "Version: " << state.version << std::endl;

  std::cout << "State: " << (state.running ? "ACTIVE" : "STOPPING")
            << std::endl;

  std::cout << "Commands processed: " << state.commands_processed << std::endl;

  std::cout << std::endl;
}

void exit_handler(RuntimeState &state, const CommandRegistry &registry) {
  (void)registry;

  std::cout << "Shutting down KnowledgeOS..." << std::endl;

  state.running = false;
}

void display_help(const CommandRegistry &registry) {
  std::cout << std::endl;
  std::cout << "Available commands:" << std::endl;

  for (const auto &entry : registry.entries) {
    std::cout << "  " << entry.first << " - " << entry.second.description
              << std::endl;
  }

  std::cout << std::endl;
}

void dispatch_command(const std::string &input, RuntimeState &state,
                      const CommandRegistry &registry) {
  std::istringstream stream(input);

  std::string command_name;

  stream >> command_name;

  if (command_name.empty()) {
    return;
  }

  state.commands_processed++;

  auto command = registry.entries.find(command_name);

  if (command == registry.entries.end()) {
    std::cout << "Unknown command: " << command_name << std::endl;

    return;
  }

  command->second.handler(state, registry);
}

void initialize(RuntimeState &state) {
  std::cout << "Booting " << state.name << "..." << std::endl;
}

int main() {
  RuntimeState runtime;

  CommandRegistry commands;

  initialize(runtime);

  register_command(commands, "help", "Display available commands",
                   help_handler);

  register_command(commands, "status", "Display runtime status",
                   status_handler);

  register_command(commands, "exit", "Shutdown KnowledgeOS", exit_handler);

  std::string input;

  while (runtime.running) {
    std::cout << "KOS:\\>" << std::flush;

    if (!std::getline(std::cin, input)) {
      std::cout << std::endl;
      break;
    }

    dispatch_command(input, runtime, commands);
  }

  return 0;
}
