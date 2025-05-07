#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
int exec[100];
pair<string, int> issue[100];
pair<string, int> execute[100];
pair<string, int> wb[100];
int k[8];

struct busy {
  pair<int, bool> fre;
  string op;
};

bool isSubstring(const std::string &word, const std::string &substring) {
  return word.find(substring) != std::string::npos;
}

class Reservation {

public:
  vector<pair<string, int>> availableStations;
  vector<pair<string, int>> noCycles;
  vector<string> a;
  vector<int> b;

  vector<string> c;
  vector<int> d;

  string name;
  vector<bool> busy[10];
  string op;
  int Vj; // RS1
  int Vk; // RS2
  int Qj;
  int Qk;
  int A;
  vector<int> cycles;
  vector<int> finish;

  vector<pair<string, int>> getStation();
  vector<pair<string, int>> getCycles();

  void parse(string, string);
};

void Reservation::parse(string file1, string file2) {
  fstream newfile;
  a.assign(10, "0");
  b.assign(10, 0);
  int i = 0;

  newfile.open(
      file1 + ".txt",
      ios::in); // open a file to perform read operation using file object
  if (newfile.is_open()) { // checking whether the file is open
    string tp;
    while (getline(newfile, tp)) {
      a[i] = tp.substr(0, tp.find(" "));
      b[i] = stoi(tp.substr(tp.find(" ") + 1));
      // cout<<b[i]<<"h"<<"\n";
      i++;
    }
  }

  newfile.close();

  c.assign(10, "0");
  d.assign(10, 0);

  i = 0;

  newfile.open(
      file2 + ".txt",
      ios::in); // open a file to perform read operation using file object
  if (newfile.is_open()) { // checking whether the file is open
    string tp;
    while (getline(newfile, tp)) {
      c[i] = tp.substr(0, tp.find(" "));
      d[i] = stoi(tp.substr(tp.find(" ") + 1));
      //  cout<<d[i]<<"h"<<"\n";

      i++;
    }
  }

  newfile.close();
}

vector<pair<string, int>> Reservation::getStation() {

  for (int i = 0; i < 10; i++) {
    availableStations.push_back(make_pair(a[i], b[i]));
    int j = 0;
    while (j < b[i]) {
      busy[i].push_back(0);
      j++;
    }
  }

  return availableStations;
}

vector<pair<string, int>> Reservation::getCycles() {
  for (int i = 0; i < 10; i++) {
    noCycles.push_back(make_pair(c[i], d[i]));
  }

  return noCycles;
}

class FP_register {

public:
  pair<string, int> reg[10];
  vector<string> operand;
  vector<string> rA;
  vector<string> rB;
  vector<string> rC;
  vector<string> imm;
  vector<string> offset;
  vector<string> label;
  vector<string> jLabel;
  pair<string, string> nameOp[100];
  pair<string, bool> flagBusy[100];
  pair<string, string> nameVj[100];
  pair<string, string> nameVk[100];

  int iteration = 0;
  int noInst = 0;
  vector<bool> taken;
  int branch = 0;
  ;
  int misprediction = 0;
  ;

  int pc = -1;

public:
  vector<string> getOperand();
  void parse();
  void issue(Reservation);
  void execute();
  void test();
};

vector<string> FP_register::getOperand() {
  fstream newfile;
  int i = 0;

  label.assign(100, "0");

  newfile.open(
      "inst.txt",
      ios::in); // open a file to perform read operation using file object
  if (newfile.is_open()) { // checking whether the file is open
    string tp;
    while (getline(newfile, tp)) {
      if (!(tp.find(":") < tp.size())) {
        operand.push_back(tp.substr(0, tp.find(" ")));

      }

      else {
        operand.push_back(
            tp.substr(tp.find(":") + 2, tp.find("x") - tp.find(":") - 3));
        label[i] = tp.substr(0, tp.find(":"));
      }

      i++;
    }
    newfile.close(); // close the file object.
  }
  return operand;
}

void FP_register::parse() {
  rA.assign(100, "0");
  rB.assign(100, "0");
  rC.assign(100, "0");
  imm.assign(100, "0");
  offset.assign(100, "0");
  jLabel.assign(100, "0");

  this->getOperand();
  fstream newfile;
  newfile.open("inst.txt", ios::in);

  int i = 0;

  if (newfile.is_open()) {
    string tp;
    while (getline(newfile, tp)) {
      if (operand[i] == "LOAD" || operand[i] == "STORE") {
        rA[i] = tp.substr(tp.find("x"), 2);
        rB[i] = tp.substr(tp.find("(") + 1, 2);
        offset[i] =
            tp.substr(tp.find("x") + 4, tp.find("(") - tp.find("x") - 4);
      }

      else if (operand[i] == "BNE") {
        rA[i] = tp.substr(tp.find("x"), 2);
        rB[i] = tp.substr(tp.find("x") + 4, 2);
        offset[i] = tp.substr(tp.find("x") + 8);
      }

      else if (operand[i] == "JAL") {
        jLabel[i] = tp.substr(tp.find(" ") + 1);
      }

      else if (operand[i] == "RET") {
        // cout << "here\n";
      }

      else if (operand[i] == "ADD" || operand[i] == "SLL" ||
               operand[i] == "NAND") {
        rA[i] = tp.substr(tp.find("x"), 2);
        rB[i] = tp.substr(tp.find("x") + 4, 2);
        rC[i] = tp.substr(tp.find("x") + 8);
      }

      else if (operand[i] == "ADDI") {
        // cout<<i<<"Hello\n";
        rA[i] = tp.substr(tp.find("x"), 2);
        rB[i] = tp.substr(tp.find("x") + 4, 2);
        imm[i] = tp.substr(tp.find("x") + 8);
      }

      else if (operand[i] == "NEG") {
        rA[i] = tp.substr(tp.find("x"), 2);
        rB[i] = tp.substr(tp.find("x") + 4, 2);
      }

      else {
        cout << operand[i] << " "
             << "Invalid Instruction - Try Again\n";
        exit(0);
      }

      i++;
    }

    newfile.close();
  }
}

void FP_register::execute() {
  this->parse();
  this->taken.assign(100, 0);
  fstream newfile;
  newfile.open("reg.txt", ios::in);

  ofstream outreg;

  ofstream outmem;

  int i = 0;

  if (newfile.is_open()) {
    string tp;
    while (getline(newfile, tp) && i <= 9) {
      reg[i].first = tp.substr(0, 2);
      reg[i].second = stoi(tp.substr(tp.find(" ")));

      // cout << reg[i].first << reg[i].second << "h" << "\n";
      i++;
    }

    newfile.close();
  }
  // cout<<"hello\n";

  fstream newfile1;
  newfile1.open("mem.txt", ios::in);

  pair<int, int> mem[131072];
  i = 0;

  //    cout<<"hello\n";

  if (newfile1.is_open()) {
    string tp;
    // cout<<"hello\n";
    while (getline(newfile1, tp)) {
      mem[i].first = stoi(tp.substr(0, tp.find(" ")));
      mem[i].second = stoi(tp.substr(tp.find(" ") + 1));
      // cout<<"h"<<mem[i].second <<"h"<<"\n";
      i++;
    }

    newfile.close();
  }

  outmem.open("mem_new.txt");
  outreg.open("reg.txt");

  pc = 0;
  for (int i = 0; i < operand.size(); i++) {
    if (operand[i] == "LOAD") {

      reg[i].second = mem[reg[i].second + stoi(imm[i])].second;
      pc += 1;

    }

    else if (operand[i] == "STORE") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {
              mem[reg[B].second + stoi(offset[i])].second = reg[A].second;
              mem[i].first = reg[B].second + stoi(offset[i]);
              outmem << mem[i].first << " "
                     << mem[reg[B].second + stoi(offset[i])].second << "\n";
              pc += 1;
            }
          }
        }
      }
    }

    else if (operand[i] == "BNE") {
      branch++;
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {
              if (reg[A].second != reg[B].second) {
                pc += stoi(offset[i]) / 4;
                misprediction++;
                taken[i] = 1;
                // cout<<reg[A].second<<" "<<reg[B].second<<"
                // "<<misprediction<<"\n";

              } else {
                pc += 1;
                taken[i] = 0;

                // cout<<reg[A].second<<" "<<reg[B].second<<"
                // "<<misprediction<<"\n";
              }
            }
          }
        }
      }
    }

    else if (operand[i] == "JAL") {
      pc += 1;

      if (jLabel[i] != "0") {
        for (int A = 0; A < operand.size(); A++) {

          if (jLabel[i] == label[A]) {
            iteration++;

            pc = A;
          }
        }
      }
    }

    else if (operand[i] == "RET") {
      pc = mem[reg[1].second].second;
    }

    else if (operand[i] == "ADD") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {
              for (int C = 0; C < 10; C++) {
                if (rC[i] == reg[C].first) {
                  reg[A].second = reg[B].second + reg[C].second;
                  pc += 1;
                }
              }
            }
          }
        }
      }
    }

    else if (operand[i] == "ADDI") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {

              reg[A].second = reg[B].second + stoi(imm[i]);
              pc += 1;
            }
          }
        }
      }
    }

    else if (operand[i] == "NEG") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {

              reg[A].second = -reg[B].second;
              pc += 1;
            }
          }
        }
      }
    }

    else if (operand[i] == "NAND") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {
              for (int C = 0; C < 10; C++) {
                if (rC[i] == reg[C].first) {
                  reg[A].second = ~(reg[B].second & reg[C].second);

                  pc += 1;
                }
              }
            }
          }
        }
      }
    }

    else if (operand[i] == "SLL") {
      for (int A = 0; A < 10; A++) {
        if (rA[i] == reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (rB[i] == reg[B].first) {
              for (int C = 0; C < 10; C++) {
                if (rC[i] == reg[C].first) {
                  reg[A].second = (reg[B].second << reg[C].second);

                  pc += 1;
                }
              }
            }
          }
        }
      }
    }

    else {
      cout << operand[i] << " "
           << "Invalid Instruction - Try Again\n";
      exit(0);
    }

    if (reg[i].first == "x0" || rA[i] == "x0") {
      reg[i].second == 0;
    }

    outreg << reg[i].first << " " << reg[i].second << "\n";
  }

  outreg.close();
  outmem.close();
}

void FP_register::issue(Reservation r) {

  int count = 2;
  r.cycles.assign(operand.size(), 0);

  vector<int> sum;
  sum.assign(100, 0);

  vector<string> name;
  name.assign(100, "0");

  int x = 0;

  for (int i = 0; i < 10; i++) {
    for (int j = 1; j <= r.getStation()[i].second; j++) {
      name[x] = r.getStation()[i].first + to_string(j);
      x++;
    }
  }

  for (int i = 0; i < x; i++) {
    flagBusy[i].first = name[i];
    flagBusy[i].second = false;
  }
}

void runs(FP_register f, Reservation r) {
  f.execute();
  f.issue(r);
  int tak = 1;
  busy b[f.operand.size()];
  pair<string, int> instExec[f.operand.size()];

  vector<int> finish;
  // finish.assign(f.operand.size(), 0);
  int A;
  int count[f.operand.size()];
  int repeat = 0;

  for (int i = 0; i < f.operand.size(); i++) {
    if (f.operand[i] == "BNE") {
      f.branch++;
      // cout<<f.branch<<"\n";
      for (int A = 0; A < 10; A++) {
        if (f.rA[i] == f.reg[A].first) {
          for (int B = 0; B < 10; B++) {
            if (f.rB[i] == f.reg[B].first) {
              if (f.reg[A].second != f.reg[B].second) {
                i += stoi(f.offset[i]) / 4;
                f.misprediction++;
                // cout<<reg[A].second<<" "<<reg[B].second<<"
                // "<<misprediction<<"\n";

              } else {
                // pc += 1;
                // cout<<reg[A].second<<" "<<reg[B].second<<"
                // "<<misprediction<<"\n";
              }
            }
          }
        }
      }
    }

    int x;
    count[i] = 1;

    if (i == 0) {
      issue[i].first = f.operand[i];
      issue[i].second = 1;

      b[i].op = issue[i].first;
      b[i].fre.second = false;

      for (int j = 0; j < 10; j++) {
        if (isSubstring(r.getCycles()[j].first, f.operand[i]))
          x = r.getCycles()[j].second;

        execute[i].first = f.operand[i];
        execute[i].second = issue[i].second + x;

        wb[i].first = f.operand[i];
        wb[i].second = execute[i].second + 1;
      }

      for (int j = 0; j < 100; j++) {
        if (isSubstring(f.flagBusy[j].first, f.operand[0])) {
          f.flagBusy[j].second == true;
          break;
        }
      }

    }

    else {
      for (int j = 0; j < i; j++) {
        if (f.operand[i] == f.operand[j])
          count[i]++;
      }

      for (int j = 0; j < 100; j++) {
        if (isSubstring(f.flagBusy[j].first, f.operand[i]) &&
                    ((count[i] - 1) < stoi(f.flagBusy[j].first.substr(
                                          f.flagBusy[j].first.length() - 1)))
                ? 1
                : 0)

          f.flagBusy[j].second = true;
        break;
      }

      for (int j = 0; j < 100; j++) {
        if (isSubstring(f.flagBusy[j].first, f.operand[i])) {
          if (((count[i]) == stoi(f.flagBusy[j].first.substr(
                                 f.flagBusy[j].first.length() - 1)))
                  ? 1
                  : 0) {
            issue[i].first = f.operand[i];
            issue[i].second = issue[i - 1].second + 1;

            b[i].op = issue[i].first;
            b[i].fre.second = false;
          }
        }

        else if (isSubstring(r.getStation()[j].first, f.operand[i]) &&
                 (count[i]) >= r.getStation()[j].second) {
          issue[i].first = f.operand[i];

          for (int k = 0; k < i; k++) {

            if (f.operand[i] == f.operand[k] && k != i &&
                wb[k].second > issue[i - 1].second) {
              // repeat++;
              // cout<<repeat<<"\n";

              issue[i].second = wb[k].second + 1;
              // cout<<f.rA[i]<<" "<<f.rA[k]<<"\n";
              count[i]--;
              // cout<<count[i]<<"\n";

              // cout<<repeat<<"\n";
            }

            else {
              issue[i].second = issue[i - 1].second + 1;

              // repeat--;
              // count[i]--;
              // cout<<i<<" "<<count[i]<<"\n";
            }
            break;
          }
          // cout<<f.operand[i]<<f.rA[i]<<" "<<j<<"
          // "<<r.getStation()[j].second<<"\n";
          // cout<<stoi(f.flagBusy[j].first.substr(f.flagBusy[j].first.length()-1));
        }
      }

      for (int j = 0; j < 100; j++) {

        // RAW
        if (f.operand[i] == r.getCycles()[j].first) {

          //          if (f.operand[i] == "JAL" && f.iteration>0)
          // {
          //     f.iteration--;

          //     if (f.jLabel[i] != "0")
          //     {
          //         for (A = 0; A < f.operand.size(); A++)
          //         {

          //             if(f.jLabel[i] == f.label[A])
          //             {
          //               i = A;
          //               //cout<<f.operand[i]<<" "<<A<<"yes\n";
          //               //cout<<f.operand[i]<<" "<<i<<" "<<f.rA[i]<<"
          //               "<<f.rB[i]<<" "<<A<<"\n"; issue[i].second =
          //               wb[A].second + 1; execute[i].second= issue[i].second
          //               + r.getCycles()[j].second;
          //               //wb[i].second = execute[i].second +1;
          //               wb[i].second = execute[i].second +1;
          //             }

          //             else
          //             {
          //               issue[i].second = issue[i-1].second + 1;

          //             }
          //         }
          //     }
          //}
          for (int o = 0; o < i; o++) {
            if (f.operand[i] == "STORE") {
              if (f.rA[i] == f.rA[o]) {
                if ((issue[i - 1].second - wb[o].second) > 0) {
                  execute[i].second = issue[i].second + r.getCycles()[j].second;
                }

                else {
                  execute[i].second = wb[o].second + r.getCycles()[j].second;
                }
              }

              else {
                execute[i].second = issue[i].second + r.getCycles()[j].second;
              }
            }

            else if (f.operand[i] == "RET") {
              if ("x1" == f.rA[o]) {
                if ((issue[i - 1].second - wb[o].second) > 0) {
                  execute[i].second = issue[i].second + r.getCycles()[j].second;
                }

                else {
                  execute[i].second = wb[o].second + r.getCycles()[j].second;
                }
              }
            }

            else if (f.rB[i] == f.rA[o] || f.rC[i] == f.rA[o]) {
              if ((i - wb[o].second) > 0) {
                execute[i].second = issue[i].second + r.getCycles()[j].second;
              }

              else {

                execute[i].second = wb[o].second + r.getCycles()[j].second;
              }
            }

            else {
              if (f.operand[o] == "BNE" && f.taken[i] == false) {
                execute[i].second = wb[o].second + r.getCycles()[j].second;
                // cout<<"Hello 354\n";
                // cout<<tak<<f.operand[i]<<wb[o].second +
                // r.getCycles()[j].second<<"\n";
                tak++;
                break;
              }

              else {
                execute[i].second = issue[i].second + r.getCycles()[j].second;
                // cout<<f.operand[i]<<" "<< execute[i].second<<"\n";
              }
            }
          }
        }
      }
    }

    b[i].op = wb[i].first;
    b[i].fre.second = true;
    b[i].fre.first = execute[i].second + 1;
    wb[i].second = b[i].fre.first;
    finish.push_back(wb[i].second);

    // cout<<"\n"<<finish.front();
    // finish.pop_back();
  }
  cout << "\nOp\t\tIssue\t  Start-Exec\t   "
          "End-Exec\t\tWB\n____________________________________________________"
          "___________________\n";

  for (int i = 0; i < f.operand.size(); i++) {
    for (int j = 0; j < f.operand.size(); j++) {
      if (f.operand[i] == r.getCycles()[j].first) {
        cout << f.operand[i] << "  \t\t" << issue[i].second << "\t\t"
             << execute[i].second - r.getCycles()[j].second + 1 << "\t\t"
             << execute[i].second << "\t\t"
             << wb[i].second /*<<" "<<b[i].fre.second*/
             << "\n____________________________________________________________"
                "___________\n";
        break;
      }
    }
  }

  std::pair<string, float> maxPair = wb[0];
  for (int i = 1; i < f.operand.size(); ++i) {
    if (wb[i].second > maxPair.second) {
      maxPair = wb[i];
    }
  }

  cout << "The total execution time: " << maxPair.second
       << "\nThe IPC: " << f.operand.size() / maxPair.second
       << "\nThe branch misprediction percentage: "
       << f.misprediction * 100 / f.branch << "%";
}

int main() {

  FP_register first;

  cout << "Hello User! Please input your reservation stations (LOAD, STORE, "
          "BNE, JAL, RET, ADD, ADDI, NEG, NAND, SLL)\n";
  string instructions[] = {"LOAD", "STORE", "BNE", "JAL",  "RET",
                           "ADD",  "ADDI",  "NEG", "NAND", "SLL"};
  int x[10];
  ofstream outputFile;
  outputFile.open("reservation.txt");

  if (!outputFile.is_open()) {
    return 1;
  }

  else {
    for (int i = 0; i < 10; i++) {
      cin >> x[i];
      outputFile << instructions[i] << " " << x[i] << std::endl;
    }
  }

  outputFile.close();

  cout << "Hello User! Please input your cycles stations (LOAD, STORE, "
          "BNE, JAL, RET, ADD, ADDI, NEG, NAND, SLL)\n";
  int y[10];
  ofstream cycles;
  cycles.open("cycles.txt");

  if (!cycles.is_open()) {
    return 1;
  }

  else {
    for (int i = 0; i < 10; i++) {
      cin >> y[i];
      cycles << instructions[i] << " " << y[i] << std::endl;
    }
  }

  cycles.close();
  Reservation R;

  R.parse("reservation", "cycles");

  runs(first, R);

  return 0;
}