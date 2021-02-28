#include "AgendaUI.hpp"
#include <csignal>
using namespace std;
AgendaUI aui;

void signalHandler(int signum) {
  cout << "Interrupt signal (" << signum << ") received." << endl;
  aui.endAgenda();
  cout << "The data has been saved!" << endl;
  system("pause");
  exit(signum);
}

int main() {
  signal(SIGINT, signalHandler);
  aui.OperationLoop();
  system("pause");
  return 0;
}
