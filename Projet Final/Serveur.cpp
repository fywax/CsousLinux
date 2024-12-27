#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "protocole.h" // contient la cle et la structure d'un message

int idQ,idShm,idSem, filsCaddie;
int fdPipe[2];
TAB_CONNEXIONS *tab;

void afficheTab();
void reponseLogin(int expediteur, int data1, char data4[100]);

typedef struct
{
  char  nom[20];
  char motDePasse[20];
} CLIENT;

int main()
{
  CLIENT client;
  int fd;
  char msg[100];
  // Armement des signaux
  // TO DO

  // Creation des ressources
  // Creation de la file de message
  fprintf(stderr,"(SERVEUR %d) Creation de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,IPC_CREAT | IPC_EXCL | 0600)) == -1)  // CLE definie dans protocole.h
  {
    perror("(SERVEUR) Erreur de msgget");
    exit(1);
  }

  // TO BE CONTINUED

  // Creation du pipe
  // TO DO

  // Initialisation du tableau de connexions
  tab = (TAB_CONNEXIONS*) malloc(sizeof(TAB_CONNEXIONS)); 

  for (int i=0 ; i<6 ; i++)
  {
    tab->connexions[i].pidFenetre = 0;
    strcpy(tab->connexions[i].nom,"");
    tab->connexions[i].pidCaddie = 0;
  }
  tab->pidServeur = getpid();
  tab->pidPublicite = 0;

  afficheTab();

  // Creation du processus Publicite (étape 2)
  // TO DO

  // Creation du processus AccesBD (étape 4)
  // TO DO

  MESSAGE m;
  MESSAGE reponse;

  while(1)
  {
  	fprintf(stderr,"(SERVEUR %d) Attente d'une requete...\n",getpid());
    if (msgrcv(idQ,&m,sizeof(MESSAGE)-sizeof(long),1,0) == -1)
    {
      perror("(SERVEUR) Erreur de msgrcv");
      msgctl(idQ,IPC_RMID,NULL);
      exit(1);
    }

    switch(m.requete)
    {
      case CONNECT :  // TO DO
                      for(int i = 0; i<6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == 0)
                        {
                          tab->connexions[i].pidFenetre = m.expediteur;
                          i = 6;
                        }
                      }

                      fprintf(stderr,"(SERVEUR %d) Requete CONNECT reçue de %d\n",getpid(),m.expediteur);
                      afficheTab();
                      break;

      case DECONNECT : // TO DO
                      for(int i = 0; i < 6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                          {
                            tab->connexions[i].pidFenetre =  0;
                            i = 6;
                          }
                      }
                      fprintf(stderr,"(SERVEUR %d) Requete DECONNECT reçue de %d\n",getpid(),m.expediteur);
                      afficheTab();
                      break;

      case LOGIN :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGIN reçue de %d : --%d--%s--%s--\n",getpid(),m.expediteur,m.data1,m.data2,m.data3);


                      if(m.data1 == 1) //nouveau client
                      {
                        strcpy(client.nom, m.data2);
                        strcpy(client.motDePasse, m.data3);

                        if((fd = open("clients.dat",O_WRONLY|O_APPEND | O_CREAT, 0644)) == -1)
                        {
                          fprintf(stderr, "Probleme d'ouverture de fichier!\n");
                          strcpy(msg,"Probleme d'ouverture de fichier!");
                          reponseLogin(m.expediteur, 0, msg);
                        }
                        else
                        {
                          write(fd, &client, sizeof(CLIENT));
                          close(fd);
                          strcpy(msg, "Utilisateur créé avec succès");
                          for(int i=0;i<6;i++)
                          {
                            if(tab->connexions[i].pidFenetre == m.expediteur) 
                            {
                              strcpy(tab->connexions[i].nom, m.data2);
                              break;
                            }
                            
                          }
                          reponseLogin(m.expediteur, 1, msg);
                        }
                      }
                      else //pas nv client
                      {
                        if((fd = open("clients.dat",O_RDONLY)) == -1)
                        {
                          fprintf(stderr, "Probleme d'ouverture de fichier!\n");
                          strcpy(msg,"Probleme d'ouverture de fichier!");
                          reponseLogin(m.expediteur, 0, msg);
                        }
                        else
                        {
                          CLIENT tempClient;
                          bool found = false;

                          while (read(fd, &tempClient, sizeof(CLIENT)) == sizeof(CLIENT)) 
                          {
                              if (strcmp(tempClient.nom, m.data2) == 0)
                              {
                                  found = true;

                                  // Vérification du mot de passe
                                  if (strcmp(tempClient.motDePasse, m.data3) == 0) 
                                  {
                                      strcpy(msg, "Connexion réussie");
                                      for(int i=0;i<6;i++)
                                      {
                                        if(tab->connexions[i].pidFenetre == m.expediteur) 
                                        {
                                          strcpy(tab->connexions[i].nom, m.data2);
                                          break;
                                        }
                                        
                                      }
                                      close(fd);
                                      reponseLogin(m.expediteur, 1, msg);
                                  } 

                                  else 
                                  {
                                      strcpy(msg, "Mot de passe incorrect");
                                      close(fd);
                                      reponseLogin(m.expediteur, 0, msg);
                                  }

                                  break; // Utilisateur trouvé, on quitte la boucle
                              }
                          }

                          if (!found) // Utilisateur non trouvé
                          {
                            strcpy(msg, "Utilisateur introuvable");
                            close(fd);
                            reponseLogin(m.expediteur, 0, msg);
                          }
                        }
                      }
                      afficheTab();
                      break; 

      case LOGOUT :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0; i < 6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur) 
                        {
                          strcpy(tab->connexions[i].nom,"");                          
                          break;
                        }
                      }
                      afficheTab();
                      break;

      case UPDATE_PUB :  // TO DO
                      break;

      case CONSULT :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
                      break;

      case ACHAT :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CADDIE :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CANCEL :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CANCEL_ALL : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);
                      break;

      case PAYER : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);
                      break;

      case NEW_PUB :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete NEW_PUB reçue de %d\n",getpid(),m.expediteur);
                      break;
    }
    //afficheTab();
  }
}

void afficheTab()
{
  fprintf(stderr,"Pid Serveur   : %d\n",tab->pidServeur);
  fprintf(stderr,"Pid Publicite : %d\n",tab->pidPublicite);
  fprintf(stderr,"Pid AccesBD   : %d\n",tab->pidAccesBD);
  for (int i=0 ; i<6 ; i++)
    fprintf(stderr,"%6d -%20s- %6d\n",tab->connexions[i].pidFenetre,
                                                      tab->connexions[i].nom,
                                                      tab->connexions[i].pidCaddie);
  fprintf(stderr,"\n");
}

void reponseLogin(int expediteur, int data1, char data4[100])
{
  MESSAGE reponse;

  reponse.expediteur = getpid();
  reponse.requete = LOGIN;
  reponse.type = expediteur;
  reponse.data1 = data1;
  strcpy(reponse.data4, data4);

  if(msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long),0) == -1)
  {
    perror("Erreur de msgsnd : 2\n");
  }
  else
  {
    printf("(Serveur %d) réponse envoyée\n", getpid());
  }

  kill(reponse.type, SIGUSR1);
}