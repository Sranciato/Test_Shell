#include "shell.h"
/**
 * attempt to execute
 */
int execute(char **args, char *path, char *envp[])
{
	pid_t pid, wpid;
	int status = 0;

	pid = fork();

	if (pid == 0)
	{
		if (args[0][0] == '/')
		{
			if (execve(args[0], args, envp) == -1)
			{
				perror("exe");
				_exit(0);
			}
		}
		if (execve(path, args, envp) == -1)
		{
			perror("exe");
			_exit(0);
		}
	}
	else if (pid < 0)
	{
		perror("exe");
		_exit(0);
	}
	else
	{
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status) && wpid > 0);
	}
	return (status);
}
/**
 * set memory
 */
void *_memset(void *s, char c, unsigned int len)
{
	char *temp = s;

	while (len > 0)
	{
		*temp = c;
		temp++;
		len--;
	}
	return (temp);
}
/**
 * copy string
 */
char *_strcpy(char *dest, char *src)
{
	int count = 0, loop;


	while (src[count] != '\0')
		count++;

	for (loop = 0; loop <= count; loop++)
		dest[loop] = src[loop];
	return (dest);
}
/**
 * concatenate two strings
 */
char *_strcat(char *dest, char *src)
{
	int loop1 = 0, loop2 = 0;

	while (*(dest + loop1) != '\0')
		loop1++;

	while (*(src + loop2) != '\0')
	{
		*(dest + loop1) = *(src + loop2);
		loop1++;
		loop2++;
	}
	return (dest);
}
/**
 * compare strings
 */
int _strcmp(char *s1, char *s2)
{
	while (*s1)
	{
		if (*s1 != *s2)
			break;
		s1++;
		s2++;
	}

	return (*s1 - *s2);
}
/**
 * compare strings
 */
int _strncmp(char *s1, char *s2, int n)
{
	while (n > 1)
	{
		if (*s1 != *s2)
			break;
		s1++;
		s2++;
		n--;
	}
	return (*s1 - *s2);
}
/**
 * get the home path
 */
char *get_home_path(char *envp[])
{
	char *path = "HOME";
	int i = 0;

	while (envp[i])
	{
		if (_strncmp(envp[i], path, 4) == 0)
			return (envp[i]);
		i++;
	}

	return (NULL);
}
/**
 * checks if file is in path
 */
char *find_path(char *path_buf[], char **args, char dest[])
{
	int i = 1, len = 0;
	DIR *dir = NULL;
	struct dirent *dp;
	int cmp;
	char *temp = NULL;

	while (args[0][len])
		len++;

	while (path_buf[i])
	{
		dir = opendir(path_buf[i]);
		if (dir == NULL)
			return (NULL);
		temp = path_buf[i];
		while ((dp = readdir(dir)) != NULL)
		{
			cmp = _strcmp(dp->d_name, args[0]);
			if (cmp == 0)
			{
				_memset(dest, 0, 1000);
				_strcpy(dest, temp);
				_strcat(dest, "/");
				_strcat(dest, args[0]);
				closedir(dir);
				return (dest);
			}
		}
		closedir(dir);
		i++;
	}
	return (NULL);
}

/**
 * parse input
 */
char **_split(char *s, char *buf[])
{
	char delim[] = " \t\r\n\v\f:=";
	char *token;
	int i = 0;

	token = strtok(s, delim);

	while (token != NULL)
	{
		buf[i] = token;
		token = strtok(NULL, delim);
		i++;
	}
	if (buf[0] == NULL)
		return (NULL);
	return (buf);
}
char **_split_newline(char *s, char *buf[])
{
	char delim[] = "\n";
	char *token;
	int i = 0;

	token = strtok(s, delim);

	while (token != NULL)
	{
		buf[i] = token;
		token = strtok(NULL, delim);
		i++;
	}

	return (buf);
}
/**
 * check for exit call
 */
void check_exit(char *args[])
{
	char *ex = "exit";
	int exit_code, check = 0;

	if ((_strncmp(args[0], ex, 4)) == 0)
		check = 1;

	if (check == 1)
	{
		if (args[1])
		{
			exit_code = atoi(args[1]);
			exit(exit_code);
		}
		else
			exit(EXIT_SUCCESS);
	}
}
/**
 * read input from user
 */
char *_read(char *buf)
{
	int rd;

	rd = read(STDIN_FILENO, buf, 1024);
	if (rd == 1 || rd == -1)
		return (NULL);
	else if (rd == 0)
	{
		write(STDOUT_FILENO, "\n", 1);
		exit(EXIT_SUCCESS);
	}
	return (buf);
}
/**
 * ignore CTR-C
 */
void sigint_handler(int sig_num)
{
	(void) sig_num;
	signal(SIGINT, sigint_handler);
}
void check_comment(char rbuffer[])
{
	int i = 0, com = 0;
	char comment[1000];
	char delim[] = "#";

	(void) comment;
	while (rbuffer[i])
	{
		if (rbuffer[i] == '#')
			com = 1;
		i++;
	}

	if (com == 1)
		strtok(rbuffer, delim);
}
void print_history(char history[][100], int hist)
{
	int i = 0;

	while (i < hist)
	{
		write(STDOUT_FILENO, history[i], sizeof(history[i]));
		i++;
	}

}

void cd(char *args[], char last_dir_buf[], char *envp[])
{
	char *home_path, home_copy[1000], *path_buf[1000];

	if (args[1])
	{
		if (_strcmp(args[1], "-") == 0)
		{
			if (chdir(last_dir_buf) != 0)
				perror("hsh");
		}
		else
		{
			getcwd(last_dir_buf, 1000);
			if (chdir(args[1]) != 0)
				perror("hsh");
		}
	}
	else
	{
		home_path = get_home_path(envp);
		_strcpy(home_copy, home_path);
		_split(home_copy, path_buf);
		getcwd(last_dir_buf, 1000);
		if (chdir(path_buf[1]) != 0)
			perror("hsh");
	}
}
void env(char *envp[])
{
	int i, j;

	for (i = 0; envp[i]; i++)
	{
		for (j = 0; envp[i][j]; j++)
			;
		write(STDOUT_FILENO, envp[i], j);
		write(STDOUT_FILENO, "\n", 1);
		fflush(stdout);
	}
}
int check_bltin(char *ar[], char his[][100], int h, char *en[], char ld_buf[])
{
	if (_strcmp(ar[0], "history") == 0)
	{
		print_history(his, h);
		return (1);
	}
	if (_strcmp(ar[0], "cd") == 0)
	{
		cd(ar, ld_buf, en);
		return (1);
	}
	if (_strcmp(ar[0], "env") == 0)
	{
		env(en);
		return (1);
	}
	return (0);
}
/**
 * get path from environ
 */
char *get_path(char *envp[])
{
	char *path = "PATH";
	int i = 0;


	while (envp[i])
	{
		if (_strncmp(envp[i], path, 4) == 0)
		{
			return (envp[i]);
		}
		i++;
	}
	return (NULL);
}
void _semi(char *sbuf[], char *envp[], int his, char hist[][100], char ldbuf[])
{
	char *args[1000], *path_buf[1000], *path, path_copy[1000];
	char dest[1000];
	int i;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(args, 0, sizeof(args));
		_split(sbuf[i], args);
		if ((check_bltin(args, hist, his, envp, ldbuf) != 0))
			continue;
		path = get_path(envp);
		_strcpy(path_copy, path);
		_split(path_copy, path_buf);

		if (args[0][0] == '/')
		{
			execute(args, args[0], envp);
			continue;
		}
		check_comment(sbuf[i]);
		find_path(path_buf, args, dest);
		execute(args, dest, envp);
	}
}
void _and(char *sbuf[], char *envp[], int his, char hist[][100], char ldbuf[])
{
	char *args[1000], *path_buf[1000], dest[1000], path_copy[1000], *path;
	int i, check = 0;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(args, 0, sizeof(args));
		_split(sbuf[i], args);
		if ((check_bltin(args, hist, his, envp, ldbuf) != 0))
			continue;
		path = get_path(envp);
		_strcpy(path_copy, path);
		_split(path_copy, path_buf);

		if (args[0][0] == '/')
		{
			check = execute(args, args[0], envp);
			if (check != 0)
				break;
			continue;
		}
		find_path(path_buf, args, dest);
		check = execute(args, dest, envp);
		if (check != 0)
			break;
	}
}
void _or(char *sbuf[], char *envp[], int his, char hist[][100], char ldbuf[])
{
	char *args[1000], *path_buf[1000], dest[1000], path_copy[1000], *path;
	int i, or;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(args, 0, sizeof(args));
		_split(sbuf[i], args);
		if ((check_bltin(args, hist, his, envp, ldbuf) != 0))
			continue;
		path = get_path(envp);
		_strcpy(path_copy, path);
		_split(path_copy, path_buf);
		if (args[0][0] == '/')
		{
			or = execute(args, args[0], envp);
			if (or == 0)
				break;
			continue;
		}
		find_path(path_buf, args, dest);
		or = execute(args, dest, envp);
		if (or == 0)
			break;
	}
}
char **_split_semiand(char *s, char *buf[])
{
	char delim[] = ";&|";
	char *token;
	int i = 0;

	token = strtok(s, delim);

	while (token != NULL)
	{
		buf[i] = token;
		token = strtok(NULL, delim);
		i++;
	}

	if (buf == NULL)
		return (NULL);

	return (buf);
}
int check_semiandor(char rb[], char *env[], int h, char hi[100][100], char ldf[])
{
	int i = 0;
	char *semi_buf[1000];

	_memset(semi_buf, 0 , sizeof(semi_buf));
	while (rb[i])
	{
		if (rb[i] == ';')
		{
			if (i == 0 || rb[i + 1] == '\0')
				return (1);
			_split_semiand(rb, semi_buf);
			_semi(semi_buf, env, h, hi, ldf);
			return (1);
		}
		else if (rb[i] == '&' && rb[i + 1] == '&')
		{
			if (i == 0 || rb[i + 1] == '\0')
				return (1);
			_split_semiand(rb, semi_buf);
			_and(semi_buf, env, h, hi, ldf);
			return (1);
		}
		else if (rb[i] == '|' && rb[i + 1] == '|')
		{
			if (i == 0 || rb[i + 1] == '\0')
				return (1);
			_split_semiand(rb, semi_buf);
			_or(semi_buf, env, h, hi, ldf);
			return (1);
		}
		i++;
	}
	return (0);
}
void file_input(char *filename, char *envp[])
{
	int fd, rd, i;
	char rbuffer[100], *semi_buf[1000], dest[1000], *path;
	char *path_buf[1000], *args[1000], path_copy[1000];

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		perror("hsh");

	rd = read(fd, rbuffer, 1024);
	if (rd == -1)
		perror("hsh");
	close(rd);
	_split_newline(rbuffer, semi_buf);
	for (i = 0; semi_buf[i]; i++)
	{
		_split(semi_buf[i], args);
		path = get_path(envp);
		_strcpy(path_copy, path);
		_split(path_copy, path_buf);
		if (args[0][0] == '/')
		{
			execute(args, args[0], envp);
			_memset(args, 0, sizeof(args));
			_memset(dest, 0, 1000);
			continue;
		}
		find_path(path_buf, args, dest);
		execute(args, dest, envp);
		_memset(args, 0, sizeof(args));
		_memset(dest, 0, 1000);
	}
	exit(0);
}
void pipe_file(char rbuffer[], char *envp[])
{
	char *path_buf[1000], *args[1000], *path;
	char *semi_buf[1000], dest[1000], path_copy[1000];
	int i;

	_split_newline(rbuffer, semi_buf);
	for (i = 0; semi_buf[i]; i++)
	{
		_split(semi_buf[i], args);
		path = get_path(envp);
		_strcpy(path_copy, path);
		_split(path_copy, path_buf);
		if (args[0][0] == '/')
		{
			execute(args, args[0], envp);
			_memset(args, 0, sizeof(args));
			_memset(dest, 0, 1000);
			continue;
		}
		find_path(path_buf, args, dest);
		execute(args, dest, envp);
		_memset(args, 0, sizeof(args));
		_memset(dest, 0, 1000);
	}
	exit(0);
}
/**
 * main function
 */
int main(int argc, char *argv[], char *envp[])
{
	char *args[1000], *path_buf[1000], history[100][100], **test, *path;
	char rbuffer[1024], dest[1000], last_dir_buf[1000], path_copy[1000];
	int hist = 1;

	_memset(history, 0, sizeof(history));
	signal(SIGINT, sigint_handler);
	if (argc == 2)
		file_input(argv[1], envp);
	while (1)
	{
	_memset(rbuffer, 0, 1024);
	_memset(args, 0, sizeof(args));
	_memset(dest, 0, 1000);
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "¯\\_(ツ)_/¯  ", 14);
	if (((_read(rbuffer)) == NULL))
		continue;
	if (isatty(STDIN_FILENO) == 0)
		pipe_file(rbuffer, envp);
	_strcpy(history[hist++], rbuffer);
	check_comment(rbuffer);
	if ((check_semiandor(rbuffer, envp, hist, history, last_dir_buf) == 1))
		continue;
	test = _split(rbuffer, args);
	if (test == NULL)
		continue;
	check_exit(args);
	if ((check_bltin(args, history, hist, envp, last_dir_buf) != 0))
		continue;
	path = get_path(envp);
	_strcpy(path_copy, path);
	_split(path_copy, path_buf);
	find_path(path_buf, args, dest);
	execute(args, dest, envp);
	}
	return (0);
}
