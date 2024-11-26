#include <unistd.h>
#include "mainwindowex3.h"
#include "ui_mainwindowex3.h"
 #include <sys/wait.h>

MainWindowEx3::MainWindowEx3(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindowEx3)
{
    ui->setupUi(this);
}

MainWindowEx3::~MainWindowEx3()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowEx3::setGroupe1(const char* Text)
{
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditGroupe1->clear();
    return;
  }
  ui->lineEditGroupe1->setText(Text);
}

void MainWindowEx3::setGroupe2(const char* Text)
{
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditGroupe2->clear();
    return;
  }
  ui->lineEditGroupe2->setText(Text);
}

void MainWindowEx3::setGroupe3(const char* Text)
{
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditGroupe3->clear();
    return;
  }
  ui->lineEditGroupe3->setText(Text);
}

void MainWindowEx3::setResultat1(int nb)
{
  char Text[20];
  sprintf(Text,"%d",nb);
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditResultat1->clear();
    return;
  }
  ui->lineEditResultat1->setText(Text);
}

void MainWindowEx3::setResultat2(int nb)
{
  char Text[20];
  sprintf(Text,"%d",nb);
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditResultat2->clear();
    return;
  }
  ui->lineEditResultat2->setText(Text);
}

void MainWindowEx3::setResultat3(int nb)
{
  char Text[20];
  sprintf(Text,"%d",nb);
  fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditResultat3->clear();
    return;
  }
  ui->lineEditResultat3->setText(Text);
}

bool MainWindowEx3::recherche1Selectionnee()
{
  return ui->checkBoxRecherche1->isChecked();
}

bool MainWindowEx3::recherche2Selectionnee()
{
  return ui->checkBoxRecherche2->isChecked();
}

bool MainWindowEx3::recherche3Selectionnee()
{
  return ui->checkBoxRecherche3->isChecked();
}

const char* MainWindowEx3::getGroupe1()
{
  if (ui->lineEditGroupe1->text().size())
  { 
    strcpy(groupe1,ui->lineEditGroupe1->text().toStdString().c_str());
    return groupe1;
  }
  return NULL;
}

const char* MainWindowEx3::getGroupe2()
{
  if (ui->lineEditGroupe2->text().size())
  { 
    strcpy(groupe2,ui->lineEditGroupe2->text().toStdString().c_str());
    return groupe2;
  }
  return NULL;
}

const char* MainWindowEx3::getGroupe3()
{
  if (ui->lineEditGroupe3->text().size())
  { 
    strcpy(groupe3,ui->lineEditGroupe3->text().toStdString().c_str());
    return groupe3;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindowEx3::on_pushButtonLancerRecherche_clicked()
{
    fprintf(stderr, "Clic sur le bouton Lancer Recherche\n");

    pid_t Fils1, Fils2, Fils3, Pere;
    int status;

    if(recherche1Selectionnee())
    {
      if ((Fils1 = fork()) == -1)
      {
          perror("Erreur lors de fork()");
          exit(0);
      }

      if (Fils1 == 0) // Processus fils
      {
          fprintf(stderr, "Groupe : %s\n", getGroupe1());
          execl("./Lecture", "Lecture", getGroupe1(), NULL);
          perror("Erreur lors de execl()"); // Si execl échoue
          exit(EXIT_FAILURE);
      }
    }

    if(recherche2Selectionnee())
    {
      if ((Fils2 = fork()) == -1)
      {
          perror("Erreur lors de fork()");
          exit(0);
      }

      if (Fils2 == 0) // Processus fils
      {
          fprintf(stderr, "Groupe : %s\n", getGroupe2());
          execl("./Lecture", "Lecture", getGroupe2(), NULL);
          perror("Erreur lors de execl()"); // Si execl échoue
          exit(EXIT_FAILURE);
      }
    }

    if(recherche3Selectionnee())
    {
      if ((Fils3 = fork()) == -1)
      {
          perror("Erreur lors de fork()");
          exit(0);
      }

      if (Fils3 == 0) // Processus fils
      {
          fprintf(stderr, "Groupe : %s\n", getGroupe3());
          execl("./Lecture", "Lecture", getGroupe3(), NULL);
          perror("Erreur lors de execl()"); // Si execl échoue
          exit(EXIT_FAILURE);
      }
    }

    while ((Pere = wait(&status)) != -1)
    {
        if (WIFEXITED(status)) // Vérifier si le fils s'est terminé normalement
        {
            fprintf(stderr, "Fils avec PID %d terminé avec le code : %d\n", Pere, WEXITSTATUS(status));

            // Associer le code de sortie au bon groupe
            if (Pere == Fils1)
                setResultat1(WEXITSTATUS(status));
            else if (Pere == Fils2)
                setResultat2(WEXITSTATUS(status));
            else if (Pere == Fils3)
                setResultat3(WEXITSTATUS(status));
        }
        else
        {
            fprintf(stderr, "Fils avec PID %d terminé de manière anormale\n", Pere);
        }
    }

    fprintf(stderr, "Tous les fils ont terminé.\n");
}

void MainWindowEx3::on_pushButtonVider_clicked()
{
  
  fprintf(stderr,"Clic sur le bouton Vider\n");

  //Vider les champs de groupe
  ui->lineEditGroupe1->clear();
  ui->lineEditGroupe2->clear();
  ui->lineEditGroupe3->clear();
  
  // Vider les champs de résultat
  ui->lineEditResultat1->clear();
  ui->lineEditResultat2->clear();
  ui->lineEditResultat3->clear();

  // Désélectionner les checkboxes
  ui->checkBoxRecherche1->setChecked(false);
  ui->checkBoxRecherche2->setChecked(false);
  ui->checkBoxRecherche3->setChecked(false);

  fprintf(stderr, "Tous les champs ont été vidés.\n");

}

void MainWindowEx3::on_pushButtonQuitter_clicked()
{
  fprintf(stderr,"Clic sur le bouton Quitter\n");

  close();
}
