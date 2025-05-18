---- FRENCH ----

Ce dossier contient un fichier C et un fichier Python. Le script Python va créer un graphe à partir des données récoltées par le script C. Ce dernier utilise perf stat pour mesurer la charge du CPU lors d'un lancement de qemu avec l'instruction WFI plutôt que WFE. Ce programme a besoin de nombreux droits (perf stat notamment, et la création de fichiers ou dossiers...), nous avons donc besoin de lancer l'éxecutable avec sudo.

Pour utiliser ce fichier il faut d'abord le compiler via : 
gcc -Wall cpu_usage.c -o cpu_usage -lm

Les paramètres sont les suivants : 
- le temps d'une seule mesure de qemu (en seconde). 
- Le deuxième est l'interval de temps entre les mesures, un interval <= 100 peut poser des problèmes de mesures, 105 est une valeur convenable (en milisecondes). 
- Le dernier paramètre est le nombre de fois qu'il faut lancer qemu pour avoir une moyenne.

Qemu est donc lancé 2 fois : une fois pour WFE et l'autre pour WFI. Ce programme prend donc (2 * temps * nombre_lancement)secondes avant de s'arreter.

Ensuite est lancé le script Python, assurez-vous d'avoir les librairies numpy et matplotlib. Dans le cas contraire, il y aura une erreur, le graphe ne sera pas calculé. Le graphe est probablement enregistré avec le nom "graph_cpu_usage.png"
    
    Pour résumer, il faut 
        - avoir matplotlib et numpy
        - utiliser sudo : sudo ./cpu_usage 10 105 10
        - consulter le graphe produit "graph_cpu_usage.png"
        
    Cette commande va donc lancer qemu_wfe et qemu_wfi 10 fois pendant 10 secondes chacun, et prendra des mesures toutes les 105ms, autrement dit, on aura environ (1000 / 105) ~= 95 mesures à la fin.
    
    
Il est bien plus simple d'utiliser sudo avant la commande que de lancer la commande et d'entrer le mot de passe alors que l'exécution a commencée.


Quant au Makefile : 
    On peut utiliser make, make clean. make datas_clean doit aussi être utilisé avec sudo devant, sinon rien ne sera effacé.

---- ENGLISH ----

This directory contains a C and a Python script. The Python script is used to put into graph an array of values (most likely floats). 
These floats represents the CPU cores usage, from 0 to 4. The C script collects the datas. It uses perf stat, and more specifically
the "task-clock" values. The program is thus meant to be launched from a superuser terminal, either by running sudo -su before or 
typing sudo ./cpu_usage ...

Let's review the parameters.
	- First parameter : time in seconds. How much time to you want qemu to run ? More likely less than 10secs.
	- Second parameter is interval time in miliseconds. Measures will be taken every interval_time miliseconds. It being <= 100 might cause overhead. I suggest 105ms.
		In the end you should have 1000*time / interval measures (*1000 because interval in ms). For exemple ./cpu_usage 10 105 will give 10000 / 105 ~= 95 measures.
	- Last parameter is how many time you want qemu to be launched in order to have more realistic values, with noise being normalized.
	
The script compars qemu with wfi and with wfe. You don't want to put high values else you will wait for hours.
An exemple : ./cpu_usage 10 105 5 will launch qemu wfi 5 times for 10 seconds, taking measurements every 105ms. 
In the end, I will have 95 values which are the mean of each takes. Same for qemu_wfe.
I will wait 10 * 5 * 2 = 100 seconds before obtaining results.

The graph writes itself as "graph_cpu_usage.png"

To resume, you need :
	- A global numpy and matplotlib librairies (sudo apt install python3-numpy, sudo apt install python3-matplotlib globally installs the librairies instead of a venv)
	- To use sudo before running the executable : sudo ./cpu_usage 10 105 5
	- Consult the generated graph.
	
The very rudamentary Makefile contains a make and make clean command. The datas_clean command needs to be ran with sudo (since you ran the executable with sudo)