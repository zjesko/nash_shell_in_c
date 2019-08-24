#include "nash.h"

int exit_nash(int n, char **args){
    printf("Exiting Nash, Goodbye\n");
    exit(1);
    return 1;
}

int pwd_nash(int n, char **args){
    printf("%s\n", pwd);
    return 1;
}

int echo_nash(int n, char **args){
    for(int i=1; i<no_tokens; i++)
        printf("%s", tokens[i]);
    printf("\n");
    return 1;
}

int cd_nash(int n, char **args){

    char cdpath[BUF_PWD];

    for(int i=0;i<n;i++){
        //   printf("%s\n", args[i]);
    } 
    if(args[1][0] == '~'){
        strcpy(cdpath, home);
        strcat(cdpath, args[1]+1);
    }
    else{
        strcpy(cdpath, args[1]);
    }
    if(chdir(cdpath) < 0)
        perror("cd Error");

    return 1;
}

int clear_nash(int n, char **args){
    printf("\033[H\033[J");
    return 1;
}


int ls_nash(int n, char **args){

    struct dirent *dp;
    DIR *dir;
    char items[BUF_PWD][BUF_PWD];
    int no_items = 0;

    if(no_tokens > 1)
        cd_nash(n, args);

    dir = opendir(".");

    while((dp = readdir(dir)) != NULL){
        if((dp -> d_name[0] == '.' && flag_hash['a']) || dp -> d_name[0] != '.'){
            strcpy(items[no_items], dp -> d_name);
            no_items++;
        }
    }

    for(int i=0;i<no_items;i++){
        if(flag_hash['l']){

            struct stat fileStat;
            if(stat(items[i], &fileStat) < 0)
                continue;
            struct passwd *pws;
            pws = getpwuid(fileStat.st_uid);
            struct group *grp;
            grp = getgrgid(fileStat.st_gid);

            printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
            printf("%2ld", fileStat.st_nlink);
            printf("%9s", pws->pw_name);
            printf("%9s", grp->gr_name);
            printf("%6ld ", fileStat.st_size);
            printf("%.12s", ctime(&fileStat.st_mtime) + 4);
            printf(" %s", items[i]);
            printf("\n");

        }
        else
            printf("%s\n", items[i]);
    }
    chdir(pwd);
    return 1;
}

int pinfo_nash(int n, char **args){


    char pinfo_path[BUF_PWD];
    char p_path[BUF_PWD];

    if(n > 1)
        sprintf(p_path, "/proc/%s/", args[1]);  
    else
        strcpy(p_path, "/proc/self/");

    strcpy(pinfo_path, p_path);
    strcat(pinfo_path, "stat");

    FILE *stat = fopen(pinfo_path, "r");
    if(stat == NULL){
        perror("statfile Error:");
        return 0;
    }

    int pid, mem = 0;
    char status, expath[BUF_PWD], pname[BUF_PWD];

    fscanf(stat, "%d %s %c", &pid, pname, &status);
    fclose(stat);

    strcat(pinfo_path, "m");

    FILE *statm = fopen(pinfo_path, "r");
    if(statm == NULL){
        perror("statfile Error:");
        return 0;
    }
    fscanf(statm, "%d", &mem);
    fclose(statm);

    strcpy(pinfo_path, p_path);
    strcat(pinfo_path, "exe"); 

    readlink(pinfo_path, expath, sizeof(expath));
    torelative(expath);

    printf("pid -> %d\n", pid);
    printf("Status -> %c\n", status);
    printf("Memory -> %d\n", mem);
    printf("Executable Path -> %s\n", expath);

    return 1;
}

int nightswatch_nash(int n, char **args){

    int time = atoi(args[1]);
    fd_set input_set;
    struct timeval timeout;

    /* Empty the FD Set */
    FD_ZERO(&input_set);
    /* Listen to the input descriptor */
    FD_SET(STDIN_FILENO, &input_set);

    if (!strcmp(args[2],"interrupt")){
		// Interupt
		printf("0\tCPU0\tCPU1\tCPU2\tCPU3\n");
		int k = 1;

		do {
			FILE *interrupt = fopen("/proc/interrupts", "r");       
			ssize_t reads;
			size_t len = 0;
			char * line = NULL;

			if (interrupt == NULL){
				perror("Error opening interrupt file: ");
				return 0;
			}

			int i = 0;

			while(i < 3 && (reads = getline(&line, &len, interrupt)) != -1) {
				i++;
			}
			long long int cpu0, cpu1, cpu2, cpu3;
			// printf("%s\n", line);

			sscanf(line, "%*lld: %lld %lld %lld %lld", &cpu0, &cpu1, &cpu2, &cpu3); 

			printf("%d\t%lld\t%lld\t%lld\t%lld\n", k, cpu0, cpu1, cpu2, cpu3);
			k++;

			fclose(interrupt);

			timeout.tv_sec = time;    // time seconds
			timeout.tv_usec = 0;    // 0 milliseconds
			select(1, &input_set, NULL, NULL, &timeout);
		}
		while(1);

		return 0;
	}
    if (!strcmp(args[2],"dirty")){
        // dirty

        do {
            FILE *meminfo = fopen("/proc/meminfo", "r");       
            ssize_t reads;
            size_t len = 0;
            char * line = NULL;

            if (meminfo == NULL){
                perror("Error opening meminfo file: ");
                return 0;
            }

            int i = 0;

            while(i < 17 && (reads = getline(&line, &len, meminfo)) != -1) {
                i++;
            }
            printf("%s", line);

            fclose(meminfo);

            timeout.tv_sec = time;    // time seconds
            timeout.tv_usec = 0;    // 0 milliseconds
            select(1, &input_set, NULL, NULL, &timeout);
        }
        while(1);
        return 0;
    }

    return 1;
}

void child_exited(int n){

    int status;
    pid_t wpid = waitpid(-1, &status, WNOHANG);

    if(wpid > 0 && WIFEXITED(status)==0){
        printf("\nProcess with pid %d exited normally\n", wpid);
    }
    if(wpid > 0 && WIFSIGNALED(status)==0){
        printf("\nProcess with pid %d exited due to a user-defined signal\n", wpid);
    }
}

int execute_program(char* command){

    int n = 0, bg = 0;
    char *tok[BUF_TOK];

    tok[n] = strtok(command, " \t\n\r\a");

    while(tok[n] != NULL){
        tok[++n] = strtok(NULL, " \t\n\r\a");
    }
    pid_t pid = fork();

    if(strcmp(tok[n-1],"&")==0){
        n--;
        tok[n] = NULL;
        bg = 1;
    } 

    if(pid < 0){
        perror("Fork failed:");
        return 0;
    }
    else if(pid == 0){
        if(bg)
            setpgid(0, 0);

        int proc = execvp(tok[0], tok);
        if(proc == -1)
            perror("Error executing:");

        exit(EXIT_FAILURE);
    }
    else {
        int status;
        if(!bg){
            do{
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        else{
            signal(SIGCHLD, child_exited);
        }
    }
    return 1; 
}


int history_nash(int k, char** args){
    
    char hist_path[BUF_PWD];
    strcpy(hist_path, home);
    strcat(hist_path, "/history.txt");

    char* l[BUF_COM];
    char c[BUF_COM];
    int n=0;
    FILE *f = fopen(hist_path, "r");
    fgets(c,BUF_COM, f);
    l[0] = strtok(c, ",");

    while(l[n] != NULL)
        l[++n] = strtok(NULL, ",");
    fclose(f);
    
    int t;
    if(k==1)
        t=10;
    else
        t=atoi(args[1]);
        
    for(int i=n-t;i<n; i++){
        printf("%s\n", l[i]);
    }

    return 1;
}