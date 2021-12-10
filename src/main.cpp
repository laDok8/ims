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
Store line[9];
Histogram Table("Table",0,25,20);
Stat

//linka = line[8]
class proces_expedice  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[8], 1);
        //finalni kontrola a expedice
        Wait(Exponential(10));
        Leave(line[8], 1);
        Table(Time - prichod);
        //opusteni systemu
    }
};


//linka = line[7]
class proces_cnc  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[7], 1);
        //CNC upravy - drazkovani a frezovani
        Wait(Exponential(25));
        Leave(line[7], 1);
        Table(Time - prichod);
        (new proces_expedice)->Activate();
    }
};

//linka = line[6]
class proces_elektro_test  : public Process {
public:
    double prichod;
    long   zmetek;
    void Behavior() {
        prichod = Time;
        Enter(line[6], 1);
        Wait(Exponential(15));
        if(Uniform(0,100) <= 5){
            Leave(line[6], 1);
            Table(Time - prichod);
            zmetek++;
        } else{
            Leave(line[6], 1);
            Table(Time - prichod);
            (new proces_cnc)->Activate();
        }
    }
};

//linka = line[5]
class proces_fotocesta2  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[5], 1);
        Wait(Exponential(70));
        Leave(line[5], 1);
        Table(Time - prichod);
        (new proces_elektro_test)->Activate();
    }
};


class proces_opticke_testovani  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[4], 1);
        Wait(7);
        Leave(line[4], 1);
        Table(Time - prichod);
        (new proces_fotocesta2)->Activate();
    }
};

class proces_fotocesta1  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[3], 1);
        Wait(Exponential(30));
        Leave(line[3], 1);
        Table(Time - prichod);
        (new proces_opticke_testovani)->Activate();
    }
};

class proces_galvaniki  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[2], 1);
        Wait(120);
        Leave(line[2], 1);
        Table(Time - prichod);
        for(int i = 0; i < 4; ++i)
            (new proces_fotocesta1)->Activate();
    }
};

class proces_prokoveni  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[1], 1);
        Wait(5);
        Leave(line[1], 1);
        Table(Time - prichod);
        for(int i = 0; i < 4; ++i)
            (new proces_galvaniki)->Activate();
    }
};

class proces_zpracovani_materialu  : public Process {
public:
    double prichod;
    void Behavior() {
        prichod = Time;
        Enter(line[0],1);
        Wait(40);
        Leave(line[0], 1);
        //Table(Time - prichod);
        for(int i = 0; i < 4; ++i)
            (new proces_prokoveni)->Activate();
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
    char const *line_name[8] = {
        "linka cnc",
        "fotocesta",
        "mokre procesy",
        "povrchove upravy",
        "testovani a baleni",
        "linka cnc",
        "fotocesta",
        "mokre procesy",
    };
    /// ./ims 2 2 2 2 2 2 2 2 2
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
    int work_hours = 8 * 60;
    Init(0,work_hours);              
    (new WorkShift)->Activate();
    Run();
    for(int i=0;i<8;i++){
        line[i].Output();
    }
    SIMLIB_statistics.Output();
    return 0;
}
