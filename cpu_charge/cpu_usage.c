#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <math.h>
#include <sys/wait.h>

int pipefd[2];

// permet de lancer séparer une commande afin de la lancer grace a execv
char **split_command(char *command);
// lance perf stat qemu
void launch_qemu(char *kernel_path, char *qemu_path, char *out, int inter);
// un fils qui lance qemu et le pere qui attend temps secondes + 1 avant de le tuer
void mesure_qemu(int temps, int intervalTps, char *out_file, char *kernel_path);
// lit un fichier record .data de perf stat et le transforme afin de n'avoir que les données task clock
void read_write_task_clock(char *in, char *out);

/*
    @param
    qemu-path : ou se trouve l'executable de qemu (eventuellement)
    kernel_path : ou trouver le kernel (normalement dans le dossier actuel)
    out : ou ecrire les données recueillies
    inter : mesures toutes les _inter_ milisecondes
*/
void launch_qemu(char *kernel_path, char *qemu_path, char *out, int inter) {
    char qemu_comm[1024];
    sprintf(qemu_comm, "./qemu-system-aarch64 -M raspi3b -kernel %s -nographic -serial null -serial mon:stdio", kernel_path);
    char perf_stat_comm[512];
    sprintf(perf_stat_comm, "perf stat -I %d -o %s", inter, out);

    const char *base_cmd = "%s %s"; // merge perf stat and qemu
    size_t needed = snprintf(NULL, 0, base_cmd, perf_stat_comm, qemu_comm) + 1;

    char *command_qemu = malloc(needed);
    if (!command_qemu) {
        perror("Erreur allocation mémoire");
        exit(EXIT_FAILURE);
    }
    sprintf(command_qemu, base_cmd, perf_stat_comm, qemu_comm); // perf stat qemu ...

    char **splitted = split_command(command_qemu);

    write(pipefd[1], "1", 1);
    close(pipefd[1]); 
    // etre sur que le pere ne me tue pas avant d'avoir mesuré jusqu'au bout
    // perf stat -p fonctionnait moins bien que perf stat ./qemu

    chdir(qemu_path);
    execvp(splitted[0], splitted); // new terminal
}

/*
    a function that splits strings into single words
    in order to call execv commands such as execvp
*/
char **split_command(char *command) {
    char *copy = strdup(command);
    char **splitted = malloc(sizeof(char *) * 20);
    int i = 0;

    char *token = strtok(copy, " ");
    while (token != NULL && i < 19) {
        splitted[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    splitted[i] = NULL;

    free(copy);
    return splitted;
}

/*
    A function that merges a int and a string.
    when reading report from perf stat, it gives
    something like 200,34 but ",34" is read as a string
    so i wrote this
*/
float contract_int_str(int cpuPrctage, char *rest) {
    int lenReste = strlen(rest) - 1;
    int resteInt = atoi(&(rest[1]));

    return cpuPrctage + (resteInt / pow(10,lenReste));
}

/*
    Measure qemu with perf stat command.
    It launches a qemu with and without wfi (wfe then),
    measurements are written into a corresponding .data file.
    (such as wfi.data or wfe.data) 
*/
void mesure_qemu(int temps, int intervalTps, char *out_file, char *kernel_path) {
    char *qemu_path = "/home/lolo/Documents/qemu-master/build/";

    pid_t pid;
    if ((pid = fork()) == 0) {
        close(pipefd[0]);
        if (setsid() == -1) { // nouvelle session pour tuer perf stat et qemu
            perror("setsid a échoué");
            exit(1);
        }
        prctl(PR_SET_PDEATHSIG, SIGKILL); // permet de tuer a la fois ce fils et qemu

        launch_qemu(kernel_path, qemu_path, out_file, intervalTps); // fils lance qemu

        perror("execvp a échoué");
        exit(1);
    }
    // le pere qui mesure le fils ne marchait pas si bien
    // desormais : le fils lance perf stat sur lui meme et le pere attend
    //mesure(temps, intervalTps, pid, out_file);
    close(pipefd[1]);
    char buf;
    read(pipefd[0], &buf, 1); // attend de recevoir un "go"("1" ici) du fils
    close(pipefd[0]);   

    sleep(temps + 1);
    kill(-pid, SIGKILL); // tue tout le groupe du fils (afin de tuer qemu créé par perf stat)
    waitpid(pid, NULL, 0);
}

/*
    Function that reads from a perf stat record and writes 
    only the task-clock values into the out file.
*/
void read_write_task_clock(char *in, char *out) {
    FILE *fin = fopen(in, "r");
    FILE *fout = fopen(out, "w");
    if (!(fin && fout)) {
        if (!fin)
            fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", in);
        else
            fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", out);
    }
    int max_size = 256;
    char buff[max_size];

    
    // format : 0.101624788   237,16 msec task-clock    #    2,372 CPUs utilized
    // OU
    // format : 0.101624788   237.16 msec task-clock    #    2,372 CPUs utilized
    // le petit point change tout
    float values[2];
    char strings[3][128];

    char cpu_prc[128];
    float f;

    while(fgets(buff, max_size - 1, fin)) {
        if (buff[0] != '#') {
            // first %f is time
            sscanf(buff, "%f %f %s %s %s", &(values[0]), &(values[1]), strings[0], strings[1], strings[2]);
            // perf stat peut varier d'affichage (400,36 ou 400.36)
            // ,36 est lu comme un string, ce qui peut décaler "task-clock" de 1
            // on vérifie alors ces 2 strings 
            if (!strcmp(strings[2], "task-clock") || !strcmp(strings[1], "task-clock")) {
                if (!strcmp(strings[2], "task-clock")) // cas de 300,36
                    f = contract_int_str(values[1], strings[0]) / 100;
                else // cas de 300.36
                    f = values[1] / 100; // / 100 pour avoir un %
                    
                sprintf(cpu_prc, "%f ", f); // writes the result
                fwrite(cpu_prc, sizeof(char), strlen(cpu_prc), fout);
            }
        }
    }
    fclose(fin);
    fclose(fout);
}

/*
    Gets a float array from a transformed file
    return the lengths of the array by the ptr leng
    
    returns the read float array
*/
float *tab_from_datas(char *in_file, int *leng) {
    FILE *fin = fopen(in_file, "r");
    if (!fin) {
        perror("Erreur ouverture fichier");
        exit(EXIT_FAILURE);
    }

    int siz = 100;
    float *tab = malloc(sizeof(float) * siz);
    if (!tab) {
        perror("Erreur allocation mémoire");
        exit(EXIT_FAILURE);
    }

    int indx = 0;
    while (fscanf(fin, "%f", &(tab[indx])) == 1) {
        ++indx;
        if (indx >= siz) {
            siz *= 2;
            tab = realloc(tab, siz * sizeof(float));
            if (!tab) {
                perror("Erreur allocation mémoire");
                free(tab);
                fclose(fin);
                exit(EXIT_FAILURE);
            }
        }
    }

    *leng = indx;

    fclose(fin);

    return tab;
}

/*
 permet de pas avoir un trop long main
 @param
 tps : combien de temps qemu est actif
 inter : la fréquence en miliseconde de recueillement de données
 nb_lancement : combien de fois qemu est lancé (*2 car wfi et wfe)
 cwd : le pwd
*/

void mesureEtTransforme(int tps, int inter, int nb_lancement, char *cwd) {
    char dossier[64];
    sprintf(dossier, "%s/cpu_datas/", cwd);
    char command_clean[75];
    sprintf(command_clean, "rm -rf %s", dossier);
    system(command_clean);

    command_clean[0] = '\0';
    sprintf(command_clean, "mkdir %s", dossier);
    system(command_clean);
    
    char name_total_wfi[128];
    char name_total_wfe[128];
    char name_wfi_filtered_task_clock[128];
    char name_wfe_filtered_task_clock[128];

    char wfi_kernel_path[256];
    sprintf(wfi_kernel_path, "%s/kernel8_wfi.img", cwd);
    char wfe_kernel_path[256];
    sprintf(wfe_kernel_path, "%s/kernel8_wfe.img", cwd);

    for (int i = 0 ; i < nb_lancement ; ++i) {
        //cree des noms de fichiers
        sprintf(name_total_wfi, "%swfi%d.data", dossier, i);
        sprintf(name_total_wfe, "%swfe%d.data", dossier, i);
        sprintf(name_wfi_filtered_task_clock, "%s%dwfi_filtered.data", dossier, i);
        sprintf(name_wfe_filtered_task_clock, "%s%dwfe_filtered.data", dossier, i);

        // wfi
        mesure_qemu(tps, inter, name_total_wfi, wfi_kernel_path); // mesure enfin le temps
        // transforme pour n'avoir que les données task-clock
        read_write_task_clock(name_total_wfi, name_wfi_filtered_task_clock);

        // wfe
        mesure_qemu(tps, inter, name_total_wfe, wfe_kernel_path);
        read_write_task_clock(name_total_wfe, name_wfe_filtered_task_clock);

        // reintialisation
        name_total_wfi[0] = '\0';
        name_total_wfe[0] = '\0';

        name_wfi_filtered_task_clock[0] = '\0';
        name_wfe_filtered_task_clock[0] = '\0';
    }
}

/// @brief lit les fichiers filtrés who (wfi ou wfe) et en fait une moyenne
/// @param who : wfi ou wfe
/// @param nb_lancement
/// @param datas_path : le pwd pour savoir ou trouver le dossier cpu_datas
void moyenne_datas(char *who, int nb_lancement, char *datas_path) {
    int leng = 0;
    int leng_max = 0;
    int lengths[nb_lancement];
    float *tab_total = NULL;

    char name_filtered_task_clock[128];

    float **tabs = malloc(sizeof(float *) * nb_lancement);

    for (int i = 0 ; i < nb_lancement ; ++i) {
        sprintf(name_filtered_task_clock, "%s/cpu_datas/%d%s_filtered.data", datas_path, i, who);
        tabs[i] = tab_from_datas(name_filtered_task_clock, &leng);
        lengths[i] = leng;

        if (leng > leng_max) {
            leng_max = leng;
        }
    }

    tab_total = calloc(leng_max, sizeof(float)); // can be 0 so calloc

    for (int i = 0 ; i < nb_lancement ; ++i) {
        for (int j = 0 ; j < lengths[i] ; ++j) {
            tab_total[j] += tabs[i][j];
        }
        // si overhead, nombre de données varies d'une mesure a l'autre
        // moyenne avec les autres tableaux (haut, bas et gauche)
        for (int j = lengths[i] ; j < leng_max ; ++j) {
            if (i == 0)
                tab_total[j] += tabs[i][j - 1] + tabs[i + 1][j];
            else {
                if (i == nb_lancement - 1)
                    tab_total[j] += tabs[i][j - 1] + tabs[i-1][j];
                else
                    tab_total[j] += tabs[i][j - 1] + tabs[i + 1][j] + tabs[i-1][j];
            } 
        }
    }

    char buff[17];
    sprintf(buff, "%s_moyenne.data", who);
    FILE *moy = fopen(buff, "w");
    if (!moy) {
        fprintf(stderr, "Erreur ouverture fichier %s", buff);
    }
    for (int i = 0 ; i < leng_max ; ++i) {
        tab_total[i] /= nb_lancement; // si trop petit peut causer des problèmes
        // et devenir des valeurs 10^17 par exemple
        if (tab_total[i] < 0 || tab_total[i] > 5) {
            if (i > 0) 
                tab_total[i] = tab_total[i - 1];
            else
                tab_total[i] = 0;
        }
            
        fprintf(moy, "%f ", tab_total[i]);
    }

    for (int i = 0 ; i < nb_lancement ; ++i) {
        free(tabs[i]);
    }

    free(tabs);
    fclose(moy);
    free(tab_total);

    fprintf(stdout, "Fin des calculs de moyennes -- veuillez vérifier le fichier %s_moyenne.data\n", who);
}

/*
    param :
        temps, cycle, nb_lancement
*/
int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Usage : ./cpu_usage temps_une_mesure interval_entre_mesure nb_lancement_qemu");
        exit(EXIT_FAILURE);
    }
    int tps = atoi(argv[1]);
    int inter = atoi(argv[2]);
    int nb_lancement = atoi(argv[3]);

    char c;
    if (inter <= 100) {
        printf("Attention, vous avez entré un interval <= 100, les données peuvent être mal recueillies, voulez-vous tout de même continuer (y / n) ? ");
        fflush(stdout);
        scanf("%c",&c);
        if (c == 'n')
            return 0;
    }

    printf("Ce programme vise à obtenir l'utilisation du CPU. \nPour cela, on utilise la commande perf stat. \nCette dernière a besoin d'avoir tous les droits et doit ainsi \nêtre lancée en superuser. Je vous prie donc d'entrer le mot de passe (si demandé). \nLancer cette commande avec sudo devant est préferable.\n");
    
    // qemu
    // sudo perf stat -p pid_qemu -o output.data -I 100
    // overhead possible : -I 100 (overhead si <= 100)
    // cela me permet d'avoir une donnée toutes les 100 milisecondes
    // autrement dit, 10 données par secondes
    // laissons tourner ce programme 10 secondes et nous obtiendrons 100 données.
    pipe(pipefd); // communication entre pere et fils pour synchronisation de la mesure
    
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    mesureEtTransforme(tps, inter, nb_lancement, cwd);

    /*
    char buff[4] = "wfi";
    moyenne_datas(buff, nb_lancement, cwd);
    sprintf(buff, "wfe");
    moyenne_datas(buff, nb_lancement, cwd);
    */
    printf("Transformation en graphe...\n");

    // graphisation
    char comm[64];
    sprintf(comm, "python3 cpu_graphs.py %d %d",tps, nb_lancement);
    int ret = system(comm);
    if (ret != 0) {
        printf("Erreur lors de l'exécution du script Python\n");
    }


    return 0;
}
