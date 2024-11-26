#include <fcntl.h>      // Pour open()
#include <unistd.h>     // Pour read(), lseek(), close()
#include <string.h>     // Pour strcmp()
#include <stdio.h>
#include <stdlib.h>
#include "FichierUtilisateur.h"

int estPresent(const char* nom) {
    int fd;  // Descripteur de fichier
    int position = 1;  // La position commence à 1
    UTILISATEUR user1;

    // Ouvre le fichier en mode lecture seule
    fd = open(FICHIER_UTILISATEURS, O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Le fichier n'existe pas");
        // Si le fichier n'existe pas, retourne -1
        return -1;
    }

    // Lire le fichier structure par structure
    while ((read(fd, &user1, sizeof(UTILISATEUR))) == sizeof(UTILISATEUR)) 
    {
        // Si l'utilisateur est trouvé
        if (strcmp(user1.nom, nom) == 0) {
            close(fd);  // Ferme le fichier
            return position;  // Retourne la position (1, 2, 3, …)
        }
        position++;  // Incrémente la position après chaque lecture
    }

    // Si l'utilisateur n'est pas trouvé, fermer le fichier et retourner 0
    close(fd);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
int hash(const char* motDePasse)
{
  int n = 0,i=1;
  int taille = strlen(motDePasse);
  int asci = 0;
  for(n=0;n<taille;n++,i++)
  {
    int temp = (int) motDePasse[n];
    temp = temp * i;
    asci += temp ;
  }
  asci = asci % 97;
  return asci;
}

////////////////////////////////////////////////////////////////////////////////////
void ajouteUtilisateur(const char* nom, const char* MDP) 
{
    int fd;
    UTILISATEUR user1;

    //ouverture en écriture unique + creation a la fin
    fd = open(FICHIER_UTILISATEURS, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    //on va à la fin
    lseek(fd, 0, SEEK_END);

    // Mettre donnée
    strncpy(user1.nom, nom, sizeof(user1.nom)-1);
    user1.nom[sizeof(user1.nom)-1] = '\0';  // 0de fin de chaine

    user1.hash = hash(MDP);

    // Écriture dans le fichier
    if((write(fd, &user1, sizeof(UTILISATEUR)) != sizeof(UTILISATEUR)))
    {
      perror("Erreur dans l'écriture");
    }

    // Ferme le fichier
    close(fd);
}
////////////////////////////////////////////////////////////////////////////////////
int verifieMotDePasse(int pos, const char* motDePasse)
{
  int fd;
  UTILISATEUR user1;

  fd = open(FICHIER_UTILISATEURS, O_RDONLY);
  if(fd==-1){
    perror("Erreur ouverture fichier");
    return -1;
  }
  lseek(fd, (pos - 1) * sizeof(UTILISATEUR), SEEK_SET);
  read(fd, &user1, sizeof(UTILISATEUR));
  if(user1.hash == hash(motDePasse))
  {
    return 1;
  }  
  close(fd);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////
int listeUtilisateurs(UTILISATEUR *vecteur) 
{
  int fd;
  int nbUtilisateurs = 0;

    fd = open(FICHIER_UTILISATEURS, O_RDONLY);  
    if (fd == -1) {
        fprintf(stderr, "Erreur à l'ouverture du fichier.\n");
        return -1;  
    }

    //lire 
    while (read(fd, &vecteur[nbUtilisateurs], sizeof(UTILISATEUR)) == sizeof(UTILISATEUR)) 
    {
      nbUtilisateurs++; 
    }

    close(fd);  
    return nbUtilisateurs; 
}
