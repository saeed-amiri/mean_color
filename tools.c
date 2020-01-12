#include "defs.h"

int get_ndirs(void)
{
  char cmd[1024]; int n; FILE *fp;
  sprintf( cmd,"ls -d */ | sed 's#/##'|wc -l" );
  fp = popen( cmd,"r" ); fscanf( fp,"%i",&n ); pclose( fp );
  return n;
}

int get_nfiles(void)
{
  char cmd[1024]; int n; FILE *fp;
  sprintf( cmd,"ls | sed 's#/##'|wc -l" );
  fp = popen( cmd,"r" ); fscanf( fp,"%i",&n ); pclose( fp );
  return n;
}

int file_exists (char * fileName)
{
  struct stat buf;
  int i = stat ( fileName, &buf );
    /* File found */
    if ( i == 0 ) {
      return 1;
    }
    return 0;
}

int get_gz(void)
{
  char cmd[1024], traj[256], snap[10], temp[10]; FILE *fp; int jj =0;
  sprintf( traj, "traj.%i.gz", RUNID ); sprintf( snap, "snap.%i.gz", RUNID );
  int i = file_exists(traj);
  if ( i == 1 ) { return 1; }
  else 
  {
    sprintf( cmd, "ls *gz | grep snap.%i.gz | sed 's#/##'",RUNID );
    fp = popen( cmd, "r" ); fscanf( fp, "%s", temp ); pclose( fp );
    printf("jj is %i\n",jj);
    jj +=1;
    if ( strcmp( snap, temp ) == 0 )
    {
      rename(temp, traj);
      printf( "get_gz: snap->traj" );
      return 1;
    }
  }
  return 0;
}

int get_time( void )
{
  char cmd[1024]; int n; FILE *fp;
  sprintf( cmd,"zgrep TIMESTEP traj.%i | wc -l", RUNID );
  fp = popen( cmd,"r" ); fscanf( fp,"%i",&n ); pclose( fp );
  return n;
}

char* get_pwd( void ){
  char cmd[1024]; FILE *fp;
  static char pwd[1024];
  sprintf( cmd,"pwd" );
  fp = popen( cmd,"r" ); fscanf( fp, "%s", pwd ); pclose( fp );
  return pwd;
}

void printwarning(char *msg, ...)
{ FILE *fp;
  char dd[1024];
  va_list argp;

  /* print error to screen... */

  va_start(argp,msg);
  vfprintf(stderr,msg,argp);
  va_end(argp);

  /* ...and to logfile... */

  fp=fopen(ERRORFILE,"a");
  va_start(argp,msg);
  fprintf(fp,"%s: RUNID=%i: ",dd,RUNID);
  vfprintf(fp,msg,argp);
  va_end(argp); fflush(fp); fclose(fp);
}

void printerror(char *msg, ...)
{ FILE *fp;
  char dd[1024];
  va_list argp;

  /* print error to screen... */

  va_start(argp,msg);
  vfprintf(stderr,msg,argp);
  va_end(argp);

  /* ...and to logfile... */

  fp=fopen(ERRORFILE,"a");
  va_start(argp,msg);
  fprintf(fp,"%s: RUNID=%i: ",dd,RUNID);
  vfprintf(fp,msg,argp);
  va_end(argp); fflush(fp); fclose(fp);

  exit(1);
}

