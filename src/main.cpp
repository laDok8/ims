#include "simlib.h"
#include <iostream>

using namespace std;

/*
cca postup vyroby:
rezani Cu desek na rozmery
CNC
finish materialu

elektrolyza


naneseni fotoresist
vystaveni svetlu
image develop
etching
wash

inspection

solder mask
baking
selective solder removal
legend print
surface finish

QA: electric test

CNC deleni desek
wash
visual inspection
vacum packing and packaging


*/
Store line[5];
Store worker[5];
Histogram Table("Table",0,25,20);

class po_zpracovani_plastu  : public Process {
public:
    double prichod;
    void Behavior() {

        prichod = Time;               
        Enter(worker[1], 2);                   
        Enter(line[2], 1);
        Wait(Normal(20,2));
        Leave(line[2], 1);
        Leave(worker[1], 2);
        Table(Time - prichod);          
        //(new Store2)->Activate();
    }
};

class zpracovani_plastu  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;               
        Enter(worker[0], 1);
        Enter(line[0],1);
        Wait(50);                     
        Leave(line[0], 1);
        Enter(line[1],1);
        Wait(Exponential(25));                     
        Leave(line[1], 1);
        Leave(worker[0], 1);                 
        //Table(Time - prichod);
        (new po_zpracovani_plastu)->Activate();
    }
};

class WorkShift  : public Process {
    void Behavior() {
        //urcuje mi pocatecni "suroviny"
        for (int i = 0; i < 50; i++)
            (new zpracovani_plastu())->Activate();
        
    };
};

int main(int argc, char **argv) {
    char const *line_name[5] = {
        "linka cnc",
        "fotocesta",
        "mokre procesy",
        "povrchove upravy",
        "testovani a baleni",
    };
    char const *worker_name[5] = {
        "delnik",
        "chemist",
        "delnik",
        "delnik",
        "delnik",
    };

    /// ./ims 2 2 2 2 2 2 2 2 2 2
    if(argc != 11){
        printf("agrumenty ...");
        exit(1);
    }

    for(int i=0;i<5;i++){
        line[i].SetCapacity(atoi(argv[i+1]));
        line[i].SetName(line_name[i]);
        worker[i].SetCapacity(atoi(argv[i+6]));
        worker[i].SetName(worker_name[i]);
    }

    Print(" model PCB - Gatema\n");
    SetOutput("model.out");
    // pracovni doba v minutach 
    int work_hours = 8 * 60;
    Init(0,work_hours);              
    (new WorkShift)->Activate();
    Run();
    for(int i=0;i<5;i++){
        line[i].Output();
        worker[i].Output();
    }          
    return 0;
}
