#include "simlib.h"
#include <iostream>

using namespace std;

Queue galvanika;
Store line[9];

//statistiky
Histogram Table("Table",0,25,20);
long statistics[9];
long dodano_do_skladu = 0;
long vzito_ze_skladu = 0;
long dokonceno = 0;
long zmetky = 0;


//linka = line[8]
class proces_expedice  : public Process {
public:
    void Behavior() {
        Enter(line[8], 1);
        //finalni kontrola a expedice
        Wait(Exponential(15));
        Leave(line[8], 1);
        dokonceno++;
        statistics[8]++;
        //opusteni systemu
    }
};

//linka = line[7]
class proces_cnc  : public Process {
public:
    void Behavior() {
        Enter(line[7], 1);
        //CNC upravy - drazkovani a frezovani
        Wait(Exponential(25));
        Leave(line[7], 1);
        (new proces_expedice)->Activate();
        statistics[7]++;
    }
};

//linka = line[6]
class proces_elektro_test  : public Process {
public:
    void Behavior() {
        Enter(line[6], 1);
        Wait(Exponential(10));
        if(Uniform(0,100) <= 5){
            Leave(line[6], 1);
            zmetky++;
        } else{
            Leave(line[6], 1);
            (new proces_cnc)->Activate();
        }
        statistics[6]++;
    }
};

//linka = line[5]
class proces_fotocesta2  : public Process {
public:
    void Behavior() {
        Enter(line[5], 1);
        Wait(Exponential(70));
        Leave(line[5], 1);
        (new proces_elektro_test)->Activate();
        statistics[5]++;
    }
};


class proces_opticke_testovani  : public Process {
public:
    void Behavior() {
        Enter(line[4], 1);
        Wait(7);
        Leave(line[4], 1);
        (new proces_fotocesta2)->Activate();
        statistics[4]++;
    }
};

class proces_fotocesta1  : public Process {
public:
    void Behavior() {
        Enter(line[3], 1);
        Wait(Exponential(50));
        Leave(line[3], 1);
        (new proces_opticke_testovani)->Activate();
        statistics[3]++;
    }
};

class proces_galvaniki  : public Process {
public:
    void Behavior() {
        Enter(line[2], 1);
        Wait(120);
        Leave(line[2], 1);

        if(galvanika.size() < 3){
            galvanika.Insert(this);
            Passivate();
        }else{
            for(int i=0; i < 3; i++){
                galvanika.GetFirst()->Activate();
            }
        }
        (new proces_fotocesta1)->Activate();
        statistics[2]++;
    }
};

class proces_prokoveni  : public Process {
public:
    void Behavior() {
        Enter(line[1], 1);
        Wait(20);
        Leave(line[1], 1);
        (new proces_galvaniki)->Activate();
        statistics[1]++;
    }
};

class proces_zpracovani_materialu  : public Process {
public:
    void Behavior() {
        dodano_do_skladu++;
        Enter(line[0],1);
        vzito_ze_skladu++;
        Wait(40);
        Leave(line[0], 1);
        for(int i = 0; i < 3; ++i)
            (new proces_prokoveni)->Activate();
         statistics[0]++;
    }
};

class WorkShift  : public Process {
    void Behavior() {
        // inicializace surovin skladu
        for (int i = 0; i < 10e5; ++i){
            (new proces_zpracovani_materialu())->Activate();
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
    /// ./ims 3 1 12 20 2 20 4 2 1
    /// ./ims 100 100 100 100 100 100 100 100 100
    if(argc != 10){
        printf("agrumenty ...");
        exit(1);
    }
    for(int i=0;i<8;i++){
        line[i].SetCapacity(atoi(argv[i+1]));
        line[i].SetName(line_name[i]);
    }

    Print(" model PCB - Gatema\n");
    SetOutput("model.out");
    // pracovni doba v minutach 
    int work_hours = 52 * 7 * 24 * 60;
    Init(0,work_hours);              
    (new WorkShift)->Activate();
    Run();

    //statistics
    for(int i=0;i<9;i++){
        line[i].Output();
        cout << "proces["<<i<<"]:  "<< line_name[i] << "  " <<  statistics[i] << endl;
    };
    cout << "dodano_do_skladu: " << dodano_do_skladu << endl;
    cout << "vzito_ze_skladu: " << vzito_ze_skladu << endl;
    cout << "dokonceno: " << dokonceno << endl;
    cout << "zmetky: " << zmetky << endl;
    return 0;
}
