#include "simlib.h"
#include <iostream>

using namespace std;

#define POCET_SMEN 1
#define DELKA_SMENY_MIN (8 * 60)

Queue galvanika;
Store line[9];

//statistiky
long statistics[9];
long vzato_ze_skladu = 0;
long dokonceno = 0;
long zmetky = 0;


class proces_expedice  : public Process {
public:
    void Behavior() override {
        Enter(line[8], 1);
        //finalni kontrola a expedice
        Wait(Exponential(5));
        Leave(line[8], 1);
        dokonceno++;
        statistics[8]++;
        //opusteni systemu
    }
};

class proces_cnc  : public Process {
public:
    void Behavior() override {
        Enter(line[7], 1);
        //CNC upravy - drazkovani a frezovani
        Wait(Exponential(15));
        Leave(line[7], 1);
        (new proces_expedice)->Activate();
        statistics[7]++;
    }
};

class proces_elektro_test  : public Process {
public:
    void Behavior() override {
        Enter(line[6], 1);
        Wait(Uniform(10, 30));
        Leave(line[6], 1);
        if(Uniform(0,100) <= 4.0){
            zmetky++;
        } else{
            (new proces_cnc)->Activate();
            statistics[6]++;
        }
    }
};

//linka = line[5]
class proces_fotocesta2  : public Process {
public:
    void Behavior() override {
        Enter(line[5], 1);
        Wait(Normal(25,2));
        Leave(line[5], 1);
        (new proces_elektro_test)->Activate();
        statistics[5]++;
    }
};


class proces_opticke_testovani  : public Process {
public:
    void Behavior() override {
        Enter(line[4], 1);
        Wait(Exponential(7));
        Leave(line[4], 1);

        if(Uniform(0,100) <= 1.0){
            zmetky++;
        } else{
            (new proces_fotocesta2)->Activate();
            statistics[4]++;
        }
    }
};

class proces_fotocesta1  : public Process {
public:
    void Behavior() override{
        Enter(line[3], 1);
        Wait(Exponential(20));
        Leave(line[3], 1);
        (new proces_opticke_testovani)->Activate();
        statistics[3]++;
    }
};

class proces_galvaniki  : public Process {
public:
    void Behavior() override {

        if(galvanika.size() < 3){
            galvanika.Insert(this);
            Passivate();
        }else{
            for(int i=0; i < 3; i++){
                galvanika.GetFirst()->Cancel();
            }
        }
        Enter(line[2], 1);
        Wait(Exponential(120));
        Leave(line[2], 1);

        for(int i=0;i<4; i++) {
            (new proces_fotocesta1)->Activate();
            statistics[2]++;
        }
    }
};

class proces_prokoveni  : public Process {
public:
    void Behavior() override{
        Enter(line[1], 1);
        Wait(Exponential(7));
        Leave(line[1], 1);
        (new proces_galvaniki)->Activate();
        statistics[1]++;
    }
};

class proces_zpracovani_materialu  : public Process {
public:
    void Behavior() override {
        Enter(line[0],1);
        vzato_ze_skladu++;
        Wait(Exponential(32));
        Leave(line[0], 1);
        for(int i = 0; i < 3; ++i) {
            (new proces_prokoveni)->Activate();
            statistics[0]++;
        }
    }
};

class Initializer  : public Process {
    void Behavior() override {
        // inicializace surovin skladu na "neomezenou" uroven
        for (int i = 0; i < 10e5; ++i){
            (new proces_zpracovani_materialu())->Activate();
        }
        // inicializace pocatecnich stavu linek z predchozich smen
        // aby na pocatku smeny nebezeli masiny pozdejsich fazi naprazdno
        int delay[] = {0,2,1,1,1,1,1,1,1};
        int amount[] = {0,15, 10,1,12,1,10,15,10};
        for(int i=1;i<9;i++){
            for(int p=0;p < amount[i];p++){
                switch(i){
                    case 0:(new proces_zpracovani_materialu)->Activate(delay[i]*p);break;
                    case 1:(new proces_prokoveni)->Activate(delay[i]*p);break;
                    case 2:(new proces_galvaniki)->Activate(delay[i]*p);break;
                    case 3:(new proces_fotocesta1)->Activate(delay[i]*p);break;
                    case 4:(new proces_opticke_testovani)->Activate(delay[i]*p);break;
                    case 5:(new proces_fotocesta2)->Activate(delay[i]*p);break;
                    case 6:(new proces_elektro_test)->Activate(delay[i]*p);break;
                    case 7:(new proces_cnc)->Activate(delay[i]*p);break;
                    case 8:(new proces_expedice)->Activate(delay[i]*p);break;
                    default:break;
                }
            }
        }

    };
};
int main(int argc, char **argv) {
    char const *line_name[9] = {
        "zpracovani materialu",
        "prokoveni",
        "galvanika",
        "fotocesta1",
        "opticke testovani",
        "fotocesta2",
        "elektricke testovani",
        "CNC",
        "expedice",
    };
    if(argc != 10){
        cout << "v argumetu pozadovana specifikace poctu linek (9) v tomto poradi:\n"
               "zpracovani materialu\n"
               "prokoveni\n"
               "galvanika\n"
               "fotocesta1\n"
               "opticke testovani\n"
               "fotocesta2\n"
               "elektricke testovani\n"
               "CNC\n"
               "expedice" << endl;
        exit(EXIT_FAILURE);
    }
    //./ims 3 1 1 1 3 1 4 2 6
    int multiplier[] = {1, 10, 20, 30, 1, 50, 4, 2, 1};
    for(int i=0;i<9;i++){
        long tmp = strtol(argv[i+1],nullptr,10) * multiplier[i];
        if(tmp <= 0){
            cerr << "nevhodna hodnota argumetu "<< i-1 << endl;
            exit(EXIT_FAILURE);
        }
        line[i].SetCapacity(tmp);
        line[i].SetName(line_name[i]);
    }

    Print(" model PCB - Gatema\n");
    SetOutput("model.out");
    Print(" model PCB - Gatema\n");
    int work_hours = POCET_SMEN * DELKA_SMENY_MIN;
    Init(0,work_hours);
    (new Initializer)->Activate();
    Run();

    //statistics
    for(int i=0;i<9;i++){
        line[i].Output();
        cout << "proces["<<i<<"]:  "<< line_name[i] << " dokonceno: " <<  statistics[i] << endl;
    }
    cout << "---------------" <<endl;
    for(int i=8;i>=0;i--){
        cout << statistics[i] << " << ";
    }
    cout << endl;

    cout << "vzato_ze_skladu: " << vzato_ze_skladu * 3 << endl;
    cout << "dokonceno: " << dokonceno << endl;
    cout << "zmetky: " << zmetky << endl;
    return EXIT_SUCCESS;
}

//            .-~~\
//           /     \ _
//           ~x   .-~_)_
//             ~x".-~   ~-.
//         _   ( /         \   _
//        ||   T  o  o     Y  ||
//       ==:l   l   <       !  I;==
//          \\   \  .__/   /  //
//           \\ ,r"-,___.-'r.//
//            }^ \.( )   _.'//.
//           /    }~Xi--~  //  \
//          Y    Y I\ \    "    Y
//          |    | |o\ \        |
//          |    l_l  Y T       |
//          l      "o l_j       !
//           \                 /
//    ___,.---^.     o       .^---.._____
//"~~~          "           ~            ~~~"
