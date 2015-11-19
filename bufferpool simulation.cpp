//
//  main.cpp
//  
//
//  Created by Yann Ouedraogo on 2015-01-20.
//  Copyright (c) 2015 Yann Ghislain Ouedraogo. All rights reserved.
//  eyo3005@umoncton.ca

#include <iostream>
#include <ctime>
#include <vector>
using namespace std;
int compteur=0;
/*Struct du bassin de tampon avec buffpool pour contenir les varible qui sont dans le bassin du tampon
 dirtydirty pour le le bit sal 
 reserved pour les indice des cases reservé par les numero entrez entre 1-15
 FileBlockNumb represente la valeur ecrite sur le tampon
 AccessCount est le compteur pour les differentes operations ecriture lecture
 */
typedef struct poolbuffer
{
    int buffpool;
    bool dirtydirty;
    bool reserved;
    int FileBlockNumb;
    int AccessCount;
}poolbuffer;
/*la class bufferSym avec les fonction necessaire tel que decrite dans l'enonce plus la fonction feeling qui afficher l'etat en
 tous temps pour constater l'etat du bassin de tampon
 */
class bufferSym
{public:
    bufferSym();
    void aquireBuffer(int);
    void releaseBuffer(int);
    void readblock(int);
    int getPointer(int);
    void makeDirty(int);
    void feeling();
    int counterIns;
    int fichiersimul[16];
    poolbuffer pool[5];
};
bufferSym::bufferSym()
{
    for (int i=1; i<16; i++)
    {
        fichiersimul[i]=i;//on initialise le tableau pour le fichier
    }
    for (int i=0; i<5; i++)
    {
        pool[i].buffpool=0;// a linitialisation rien est ecris dans le block du buffer
        pool[i].dirtydirty=false;// le bit sal est a 0
        pool[i].FileBlockNumb=0;// le tableau de pour les block reserver est a zero
        pool[i].AccessCount=0;// le compteur d'acces est a 0
        pool[i].reserved=false;// la reservation est 0
    }
    counterIns=-1;
}
/*
 La logic ici est de faire une reservation du debut du tableau a la fin de façon sequentiel mais si tous les block on deja ete
 reserve au moins une fois on utilise l'Heuristique deux et on insere en fonction du comportement du compteur et on genere une erreur si tous les bloc du buffer sont reserve
 */
void bufferSym::aquireBuffer(int numbBlock)
{
    int indice=0;
    indice=fichiersimul[numbBlock];
    for (int i=0; i<5; i++)
    {
        if(pool[i].FileBlockNumb==0)
        {
            pool[i].FileBlockNumb=indice;
            pool[i].reserved=true;
            pool[i].dirtydirty=false;
            pool[i].AccessCount++;
            counterIns++;
            break;
        }
        else if(counterIns==4)
        {
            cout<<"Le bassin est plein Veillez Liberez un/des block svp"<<endl;
            break;
        }
        else if(pool[i].reserved==false)
        {
            if (pool[i].AccessCount<=pool[i+1].AccessCount)
            {
                pool[i].FileBlockNumb=indice;
                pool[i].reserved=true;
                pool[i].dirtydirty=false;
                pool[i].AccessCount++;
                counterIns++;
                break;
            }
        }
    }
     cout<<"\n";
}
/*
 L'esprit ici est de verifier le bit sal avant de liberer la memoire si il est a 1 on ecrit dans le fichier si 
 il est 0 on libere le block normalement en mettant le bit de reservation a false
 */
void bufferSym:: releaseBuffer(int numBlock)
{
    if (pool[numBlock].dirtydirty==false)
    {
        pool[numBlock].reserved=false;
        counterIns--;
    }
    else
    {
        int manip=pool[numBlock].FileBlockNumb;
        fichiersimul[manip]=pool[numBlock].buffpool;
    }
}
/*
 Read block prend en parametre le block pour lequel le block du fichier est reserve pour l'ecrire dans le buffer
 et increment le compteur d'Acces
 */
void bufferSym:: readblock(int numBlock)
{
    int cpt2=0;
    for (int i=0; i<5; i++)
    {
        if (pool[i].FileBlockNumb==numBlock)
        {
            pool[i].buffpool=numBlock;
            pool[i].AccessCount++;
            break;
        }
        else if(cpt2==4)
        {
            cout<<"Vous n'avez pas reserve de block pour ce fichier"<<endl;
            break;
        }
        cpt2++;
    }
    cout<<"\n";
}
/*
 Make dirty utilise la libraire ctime pour aider rand a generer des nombre aleatoire
 on verifie que le bit sal est bien a zero avant d'ecrire bien sur pour que sa fonctionne correctement il faut respecter l'odre 
 des commande du menu
 */
void bufferSym::makeDirty(int numBlock)
{
    int alea,cpt3=0;
    srand(time(NULL));
    alea=rand()%100;
    for (int i=0; i<5; i++)
    {
        if(pool[i].dirtydirty==false)
        {
            pool[numBlock].buffpool=alea;
            pool[numBlock].dirtydirty=true;
            cpt3++;
            break;
        }
        else if(cpt3==4)
        {
            cout<<"Le bassin est plein Veillez Liberez un/des block svp"<<endl;
            break;
        }
    }
    cout<<"\n";
}
/*
 fonction qui retourne un pointeur pour l'Afficher dans la console un principe tres simple 
 on prend l'adresse du contenue du block dans la memoire on l'affiche et on le retourne
 */
int bufferSym::getPointer(int numBlock)
{
    int *ptr;
    ptr=&pool[numBlock].buffpool;
    cout<<*ptr<<"\n";
    return *ptr;
}
/*
 feeling est une fonction pour montrer comment le buffer se comporte c'est principalement l'interface que l'utilisateur 
 veurra
 */
void bufferSym::feeling()
{
    cout<<"-*-*-*-*-*-*-*-*-*-*-*-*-*Etat du buffer-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
    for (int i=0; i<5; i++)
    {
        cout<<"Block"<<i<<": "<<pool[i].buffpool<<"|"<<"Reserve"<<i<<": "<<pool[i].FileBlockNumb<<"|"<<"DirtyBit"<<i<<": "<<pool[i].dirtydirty<<"|"<<"reservation Booleen"<<i<<": "<<pool[i].reserved<<"|"<<"Acces Counter"<<i<<": "<<pool[i].AccessCount<<""<<endl;
    }
    cout<<"-*-*-*-*-*-*-*-*-*-*-*-*-*Etat du Fichier Simulée-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
    for (int i=1; i<16; i++)
    {
            cout<<fichiersimul[i]<<"|"<<"  ";
    }
    cout<<"\n";
}

int main()
{
    cout<<"Simulation de Bassin de tampon 1.0"<<endl;
    cout<<"Ouedraogo Yann Ghislain NI: A00167123"<<endl;
    int numberchoice;
    bool doitagain=true;
    bufferSym Simulator;
    Simulator.feeling();
    do{
        cout<<"================================================================"<<endl;
        cout<<"1. Réserver le bloc"<<endl;
        cout<<"2. Libérer le bloc"<<endl;
        cout<<"3. Lire la valeur du fichier pour ce bloc dans le tampon"<<endl;
        cout<<"4. Afficher la valeur de ce bloc dans la console"<<endl;
        cout<<"5. Écrire une valeur aléatoire dans le bloc"<<endl;
        cout<<"6. Arret du programme"<<endl;
        cout<<"================================================================="<<endl;
        do
        {
            cout<<"Entrez votre choix s'il vous plait:";
            cin>>numberchoice;
        }
        while (numberchoice<1 || numberchoice>6);
        switch(numberchoice)
        {
            case 1:
                int numbBlock;
                cout<<"Choisissez un bloc de 1 à 15"<<endl;
                cout<<"Entrez le numero du block a reserve"<<endl;
                cin>>numbBlock;
                if (numbBlock<0||numbBlock>15)
                {
                    cout<<"Entrez svp un nombre entre 1 et 15"<<endl;
                    break;
                }
                Simulator.aquireBuffer(numbBlock);
                Simulator.feeling();
                break;
            case 2:
                int numbBlock2;
                cout<<"Choisissez un bloc du buffer à liberer choix entre 0 à 4 "<<endl;
                cout<<"Entrez le numero du block a liberer"<<endl;
                cin>>numbBlock2;
                if (numbBlock2<0||numbBlock2>4)
                {
                    cout<<"Entrez svp un nombre entre 0 et 4"<<endl;
                    break;
                }
                Simulator.releaseBuffer(numbBlock2);
                Simulator.feeling();
                break;
            case 3:
                int numbBlock3;
                cout<<"Choisissez un bloc de 1 à 15"<<endl;
                cout<<"Entrez le numero de block à lire dans le tampom"<<endl;
                cin>>numbBlock3;
                if (numbBlock3<0||numbBlock3>15)
                {
                    cout<<"Entrez svp un nombre entre 1 et 15"<<endl;
                    break;
                }
                Simulator.readblock(numbBlock3);
                Simulator.feeling();
                break;
            case 4:
                int numbBlock4;
                cout<<"Choisissez le bloc du buffer a pointer choix entre 0 à 4 "<<endl;
                cout<<"Entrez le numero de block à lire dans le tampom"<<endl;
                cin>>numbBlock4;
                if (numbBlock4<0||numbBlock4>4)
                {
                    cout<<"Entrez svp un nombre entre 0 et 4"<<endl;
                    break;
                }
                Simulator.getPointer(numbBlock4);
                Simulator.feeling();
                break;
            case 5:
                int numbBlock5;
                cout<<"Choisissez un bloc du buffer à ecrire aleatoirement choix entre 0 à 4 "<<endl;
                cout<<"Entrez le numero de block ou il faut ecrire la valeur aléatoire"<<endl;
                cin>>numbBlock5;
                if (numbBlock5<0||numbBlock5>4)
                {
                    cout<<"Entrez svp un nombre entre 0 et 4"<<endl;
                    break;
                }
                Simulator.makeDirty(numbBlock5);
                Simulator.feeling();
                break;
            case 6:
                cout<<"Arret du programe.."<<endl;
                doitagain=false;
                break;
        }
    }
    while(doitagain);
    
    return 0;
}
