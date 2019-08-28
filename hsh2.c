#include "shell.h"
char *_memcpy(char *dest, char *src, unsigned int n)
{
	unsigned int loop = 0;

	while (loop < n)
	{
		dest[loop] = src[loop];
		loop++;
	}
	return (dest);
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
int _strlen(char *s)
{
	int i;

	for (i = 0; s[i]; i++)
		;
	return (i);
}
int _atoi(char *s)
{
	int loop, digitCount = 0, neg = 1, start = 0, multiple = 1;

	int temp, number = 0;

	for (loop = 0; s[loop] != '\0'; loop++)
	{
		if (s[loop] == '-')
			neg *= -1;
		if (s[loop] >= 48 && s[loop] <= 57)
		{
			digitCount++;
			if (digitCount > 1)
				multiple *= 10;
			if (!(s[loop + 1] >= 48 && s[loop + 1] <= 57))
				break;
		}
	}
	for (start = (loop + 1) - digitCount; start < loop + 1; start++)
	{
		temp = (s[start] - '0') * multiple;
		number += temp;
		multiple /= 10;
	}
	number *= neg;
	return (number);
}
int _itoa(int n, char buf[])
{
	int i = 0, temp, start = 0, len;
	char buffer[1000];

	if (n == 0)
	{
		buf[0] = '0';
		return (1);
	}
	if (n < 0)
	{
		buffer[i] = '-';
		i++;
		n = -n;
	}
	while (n)
	{
		temp = n % 10;

		if (temp > 9)
		{
			buffer[i] = (temp - 10) + 'A';
			i++;
		}
		else if (temp < 10)
		{
			buffer[i] = temp + '0';
			i++;
		}
		n = n / 10;
	}
	i--;
	len = i;
	while (i >= 0)
	{
		buf[start] = buffer[i];
		start++;
		i--;
	}
	return (len);
}
void _error(buf_struct *a, char buffer[])
{
	write(STDOUT_FILENO, a->argv[0], _strlen(a->argv[0]));
	write(STDOUT_FILENO, ": ", 2);

	write(STDOUT_FILENO, buffer, 4);
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, a->args[0], _strlen(a->args[0]));
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, "not found", 9);
	write(STDOUT_FILENO, "\n", 1);
}
void error_cd(buf_struct *a, char buffer[])
{
	write(STDOUT_FILENO, a->argv[0], _strlen(a->argv[0]));
	write(STDOUT_FILENO, ": ", 2);
        write(STDOUT_FILENO, buffer, 4);
        write(STDOUT_FILENO, ": ", 2);
        write(STDOUT_FILENO, "cd: can't cd to ", 16);
        write(STDOUT_FILENO, a->args[1], _strlen(a->args[1]));
        write(STDOUT_FILENO, "\n", 1);
}
/**
 * attempt to execute
 */
int execute(buf_struct *a, char *path)
{
	pid_t pid;
	int status = 0;
	char buffer[1000];

	pid = fork();
	_memset(buffer, 0, 1000);
	_itoa(a->hist, buffer);

	if (pid == 0)
	{
		if (a->args[0][0] == '/')
		{
			if (execve(a->args[0], a->args, a->envp) == -1)
			{
				_error(a, buffer);
				_exit(errno);
			}
		}
		if (execve(path, a->args, a->envp) == -1)
		{
			_error(a, buffer);
			_exit(errno);
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
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return (status);
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
	int i = 0;
	DIR *dir = NULL;
	struct dirent *dp;
	int cmp;
	char *temp = NULL;

	for (i = 1; path_buf[i]; i++)
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
void exit_error(char *args[], char buffer[])
{
	write(STDOUT_FILENO, "hsh: ", 5);
	write(STDOUT_FILENO, buffer, 4);
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, args[0], _strlen(args[0]));
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, "illegal number", 15);
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, args[1], _strlen(args[0]));
	write(STDOUT_FILENO, "\n", 1);
}
/**
 * check for exit call
 */
void check_exit(char *args[], int hist)
{
	char *ex = "exit";
	int exit_code, check = 0;
	char buffer[1000];

	_memset(buffer, 0, 1000);
	_itoa(hist, buffer);

	if ((_strncmp(args[0], ex, 4)) == 0)
		check = 1;

	if (check == 1)
	{
		if (args[1])
		{
			exit_code = _atoi(args[1]);
			if (exit_code >= 0 && exit_code <= 2147483647)
				exit(exit_code);
			else
			{
				exit_error(args, buffer);
				exit(2);
			}
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
	int i = 0, len;
	char buffer[1000];

	while (i < hist)
	{
		_memset(buffer, 0, 1000);
		if (i == 0)
		{
			write(STDOUT_FILENO, "    ", 4);
			write(STDOUT_FILENO, "0", 1);
		}
		else if (i < 10)
		{
			len = _itoa(i, buffer);
			if (len < 4)
			{
				while (len++ < 4)
					write(STDOUT_FILENO, " ", 1);
			}
			write(STDOUT_FILENO, buffer, 4);
		}
		else if (i > 9)
		{
			len = _itoa(i, buffer);
			if (len < 4)
			{
				while (len++ < 4)
					write(STDOUT_FILENO, " ", 1);
			}
			write(STDOUT_FILENO, buffer, 4);
		}
		write(STDOUT_FILENO, "  ", 2);
		write(STDOUT_FILENO, history[i], sizeof(history[i]));
		i++;
	}
}
char *change_old_pwd(char *envp[], char ldbuf[], char opwd[])
{
	char *path = "OLDPWD", buffer[1000];
	int i = 0;

	while (envp[i])
	{
		if (_strncmp(envp[i], path, 6) == 0)
		{
			_strcpy(buffer, ldbuf);
			_memset(opwd, 0, 1000);
			_strcpy(opwd, "OLDPWD=");
			_strcat(opwd, buffer);
			envp[i] = opwd;
		}
		i++;
	}

	return (NULL);
}
char *get_old_pwd(char *envp[], char opwd[])
{
	int i = 0;
	char *path = "OLDPWD";

	while (envp[i])
	{
		if (_strncmp(envp[i], path, 6) == 0)
		{
			_strcpy(opwd, envp[i]);
			return (opwd);
		}
		i++;
	}
	return (NULL);
}
void cd_helper(char cwd[], char pwd[], char *envp[])
{
	int i = 0;

	getcwd(cwd, 1000);
	_strcat(pwd, cwd);
	while (envp[i])
	{
		if (_strncmp(envp[i], "PWD", 3) == 0)
			envp[i] = pwd;
		i++;
	}
}
void cd_home(buf_struct *a, char *pb[])
{
	char cwd[1000], temp[1000], *home_path, home_copy[1000];

	_strcpy(temp, a->ldbuf);
	home_path = get_home_path(a->envp);
	_strcpy(home_copy, home_path);
	_split(home_copy, pb);
	if (chdir(pb[1]) != 0)
		perror("hsh");
	else
	{
		getcwd(a->ldbuf, 1000);
		cd_helper(cwd, a->pwdb, a->envp);
		change_old_pwd(a->envp, temp, a->opwdb);
	}
}
void cd(buf_struct *a)
{
	char *path_buf[1000];
	char buffer[1000], cwd[1000], temp[1000], *pwd_split[1000];

	_memset(a->pwdb, 0, 1000);
	_strcpy(a->pwdb, "PWD=");
	_memset(buffer, 0, 1000);
	_itoa(a->hist, buffer);
	if (a->args[1])
	{
		if (_strcmp(a->args[1], "-") == 0)
		{
			_strcpy(temp, a->ldbuf);
			get_old_pwd(a->envp, a->opwdb);
			_split(a->opwdb, pwd_split);
			if (chdir(pwd_split[1]) != 0)
				chdir(a->ldbuf);
			else
			{
				getcwd(a->ldbuf, 1000);
				cd_helper(cwd, a->pwdb, a->envp);
				change_old_pwd(a->envp, temp, a->opwdb);
				write(STDOUT_FILENO, a->ldbuf, _strlen(a->ldbuf));
				write(STDOUT_FILENO, "\n", 1);
			}
		}
		else
		{
			_strcpy(temp, a->ldbuf);
			if (chdir(a->args[1]) != 0)
			{
				error_cd(a, buffer);
				_exit(errno);
			}
			else
			{
				getcwd(a->ldbuf, 1000);
				cd_helper(cwd, a->pwdb, a->envp);
				change_old_pwd(a->envp, temp, a->opwdb);
			}
		}
	}
	else
		cd_home(a, path_buf);
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
int check_bltin(buf_struct *a)
{
	if (_strcmp(a->args[0], "history") == 0)
	{
		print_history(a->history, a->hist);
		return (1);
	}
	if (_strcmp(a->args[0], "cd") == 0)
	{
		cd(a);
		return (1);
	}
	if (_strcmp(a->args[0], "env") == 0)
	{
		env(a->envp);
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
void _semi(char *sbuf[], buf_struct *a)
{
	char *path;
	char dest[1000];
	int i;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(a->args, 0, sizeof(a->args));
		_split(sbuf[i], a->args);
		if ((check_bltin(a) != 0))
			continue;
		path = get_path(a->envp);
		_strcpy(a->path_copy, path);
		_split(a->path_copy, a->path_buf);

		if (a->args[0][0] == '/')
		{
			execute(a, NULL);
			continue;
		}
		check_comment(sbuf[i]);
		find_path(a->path_buf, a->args, dest);
		execute(a, dest);
	}
}
void _and(char *sbuf[], buf_struct *a)
{
	char dest[1000], *path;
	int i, check = 0;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(a->args, 0, sizeof(a->args));
		_split(sbuf[i], a->args);
		if ((check_bltin(a) != 0))
			continue;
		path = get_path(a->envp);
		_strcpy(a->path_copy, path);
		_split(a->path_copy, a->path_buf);

		if (a->args[0][0] == '/')
		{
			check = execute(a, a->args[0]);
			if (check != 0)
				break;
			continue;
		}
		find_path(a->path_buf, a->args, dest);
		check = execute(a, dest);
		if (check != 0)
			break;
	}
}
void _or(char *sbuf[], buf_struct *a)
{
	char dest[1000], *path;
	int i, or;

	for (i = 0; sbuf[i]; i++)
	{
		_memset(dest, 0, 1000);
		_memset(a->args, 0, sizeof(a->args));
		_split(sbuf[i], a->args);
		if ((check_bltin(a) != 0))
			continue;
		path = get_path(a->envp);
		_strcpy(a->path_copy, path);
		_split(a->path_copy, a->path_buf);
		if (a->args[0][0] == '/')
		{
			or = execute(a, a->args[0]);
			if (or == 0)
				break;
			continue;
		}
		find_path(a->path_buf, a->args, dest);
		or = execute(a, dest);
		if (or == 0)
			break;
	}
}
char **_split_semiandor(char *s, char *buf[])
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
int check_semiandor(buf_struct *a)
{
	int i = 0;
	char *semi_buf[1000];

	_memset(semi_buf, 0, sizeof(semi_buf));
	while (a->rbuf[i])
	{
		if (a->rbuf[i] == ';')
		{
			if (i == 0 || a->rbuf[i + 1] == '\0')
				return (1);
			_split_semiandor(a->rbuf, semi_buf);
			_semi(semi_buf, a);
			return (1);
		}
		else if (a->rbuf[i] == '&' && a->rbuf[i + 1] == '&')
		{
			if (i == 0 || a->rbuf[i + 1] == '\0')
				return (1);
			_split_semiandor(a->rbuf, semi_buf);
			_and(semi_buf, a);
			return (1);
		}
		else if (a->rbuf[i] == '|' && a->rbuf[i + 1] == '|')
		{
			if (i == 0 || a->rbuf[i + 1] == '\0')
				return (1);
			_split_semiandor(a->rbuf, semi_buf);
			_or(semi_buf, a);
			return (1);
		}
		i++;
	}
	return (0);
}
void pipe_file(buf_struct *a)
{
        char *path_buf[1000], *semi_buf[1000], *path;
        int i, ext;

        _split_newline(a->rbuf, semi_buf);
        for (i = 0; semi_buf[i]; i++)
        {
		a->hist++;
                _split(semi_buf[i], a->args);
                path = get_path(a->envp);
                _strcpy(a->path_copy, path);
                _split(a->path_copy, path_buf);
                if (a->args[0][0] == '/')
                {
                        execute(a, a->args[0]);
                        _memset(a->args, 0, sizeof(a->args));
                        _memset(a->dest, 0, 1000);
                        continue;
                }
                find_path(path_buf, a->args, a->dest);
                ext = execute(a, a->dest);
                _memset(a->args, 0, sizeof(a->args));
                _memset(a->dest, 0, 1000);
        }
        exit(ext);
}
void file_input(buf_struct *a)
{
        int fd, rd, i, hist  = 0;
        char *semi_buf[1000], *path, *path_buf[1000];

        fd = open(a->argv[0], O_RDONLY);
        if (fd == -1)
                perror("hsh");

        rd = read(fd, a->rbuf, 1024);
        if (rd == -1)
                perror("hsh");
        close(rd);
        _split_newline(a->rbuf, semi_buf);
        for (i = 0; semi_buf[i]; i++)
        {
                hist++;
                _split(semi_buf[i], a->args);
                path = get_path(a->envp);
                _strcpy(a->path_copy, path);
                _split(a->path_copy, path_buf);
                if (a->args[0][0] == '/')
                {
                        execute(a, a->args[0]);
                        _memset(a->args, 0, sizeof(a->args));
                        _memset(a->dest, 0, 1000);
                        continue;
                }
                find_path(path_buf, a->args, a->dest);
                execute(a, a->dest);
                _memset(a->args, 0, sizeof(a->args));
                _memset(a->dest, 0, 1000);
        }
        exit(0);
}
buf_struct *make_struct_a(char *envp[], char *argv[])
{
	static buf_struct a;

	a.envp = envp;
	a.argv = argv;
	a.hist = 0;
	_memset(a.path_buf, 0, sizeof(a.path_buf));
	_memset(a.history, 0, sizeof(a.history));

	return (&a);
}
void main_loop(buf_struct *a)
{
	char **test, *path;

	_memset(a->ldbuf, 0, 1000);
	getcwd(a->ldbuf, 1000);
	while (1)
	{
		_memset(a->rbuf, 0, 1024);
		_memset(a->args, 0, sizeof(a->args));
		_memset(a->dest, 0, 1000);
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);
		if (((_read(a->rbuf)) == NULL))
			continue;
		if (isatty(STDIN_FILENO) == 0)
			pipe_file(a);
		_strcpy(a->history[a->hist++], a->rbuf);
		check_comment(a->rbuf);
		if ((check_semiandor(a) == 1))
			continue;
		test = _split(a->rbuf, a->args);
		if (test == NULL)
			continue;
		check_exit(a->args, a->hist);
		if ((check_bltin(a) != 0))
			continue;
		path = get_path(a->envp);
		_strcpy(a->path_copy, path);
		_split(a->path_copy, a->path_buf);
		find_path(a->path_buf, a->args, a->dest);
		execute(a, a->dest);
	}
}

int main(int argc, char *argv[], char *envp[])
{
	buf_struct *a;

	a = make_struct_a(envp, argv);

	signal(SIGINT, sigint_handler);
	if (argc == 2)
		file_input(a);
	main_loop(a);

	return (0);
}

