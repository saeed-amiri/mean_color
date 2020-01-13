#include "defs.h"
// file pointers
static char color[11],mode[11];
static int NTIME=351, NMODE=252, NPROP=3;
static FILE *TR;
void read_dirs(void){
  int nf,ndir,i;
  char (*dir_name)[5];
  char cmd[1024],pmd[1024],dir_list[512]; 
  FILE *fp;

  nf = get_ndirs();
  dir_name = malloc( nf * sizeof(dir_name) );
  sprintf( pmd,"ls -d */ | sed 's#/##'" ); fp = popen( pmd,"r" ); 

  ndir = 0;
  while ( fscanf(fp,"%s",dir_list) == 1 )
  {
    chdir( dir_list );
      //! check if PARAM.base file exist in the directory
      int jfile = file_exists( "PARAM.base" );
      if (jfile == 1) {
        strcpy( dir_name[ndir], dir_list ); ndir += 1; 
      }
    chdir( ".." );
  }
  printf( "\nmain: number of run dirs: %i:\n", ndir ); pclose(fp);
  int ilen;
  for (ilen = 0; ilen < ndir; ilen++)
  {
    printf("%s, strlen: %lu\n",dir_name[ilen],strlen(dir_name[ilen]));
  }
  sprintf(color,"color.%i.gz",RUNID);
  sprintf(mode,"MODEINFO.%i",RUNID);
  dir_name = realloc(dir_name, ndir * sizeof(dir_name));
  int idir,ikick,j,ind;
  static char kick[4]; int rows=NTIME,columns=NMODE*NPROP+1,tmpNT,tmpNM,tmpNP;
  for ( ikick = 1; ikick < 21; ikick++)
  {
    double **DUMP=(double**)malloc(rows*sizeof(double*));
    for(i=0;i<rows;i++)DUMP[i]=(double*)malloc(columns*sizeof(double));
    for(i=0;i<rows;i++)for(j=0;j<columns;j++)DUMP[i][j]=0.0;
    double **MINFO=(double**)malloc(NMODE*sizeof(double*));
    for(i=0;i<NMODE;i++)MINFO[i]=(double*)malloc(NPROP*sizeof(double));
    for(i=0;i<NMODE;i++)for(j=0;j<NPROP;j++)MINFO[i][j]=0.0;
    char buffer[102240];char muffer[1024];
    snprintf( kick, 4, "%f", ikick * 0.1 );
    float kk = atof(kick);
    for ( idir = 0; idir < ndir; idir++ )
    {
      char cwd[1024]; strcpy( cwd, get_pwd() );
      char here[2056],Mhere[2056];
      char *pch,*state;
      strcpy(here,strcat(strcat(strcat(cwd,"/"),dir_name[idir]),"/") );
      strcat(strcat(strcat(here,kick),"/"),color);
      sprintf(cmd,"zcat %s",here); TR=popen(cmd,"r");
      fscanf(TR,"%i %i %i",&tmpNM,&tmpNT,&tmpNP);
      if(tmpNT!=NTIME || tmpNM!=NMODE || tmpNP!=NPROP)
      {
        printf("wrong first line in: %s\n",here);
        printf("%i %i %i\n",tmpNT,tmpNM,tmpNP);
        exit(EXIT_FAILURE);
      }
      while (fgets(buffer,sizeof(buffer),TR)!=NULL)
      {
        pch=__strtok_r(buffer," \n",&state);
        int colloop=0;
        while (pch!=NULL)
        {
          if (colloop==0) ind=(atoi(pch)/1000);
          DUMP[ind][colloop]+=atof(pch);
          pch=__strtok_r(NULL," \t\n",&state);
          colloop++;
        }
      }
      pclose(TR);
      strcpy(Mhere,cwd);
      strcat(strcat(strcat(Mhere,kick),"/"),mode);
      TR=fopen(Mhere,"r");
      int iind=0;
      while (fgets(muffer,sizeof(muffer),TR)!=NULL)
      {
        pch=__strtok_r(muffer," \n",&state);
        int jind=0;
        while (pch != NULL)
        {
          MINFO[iind][jind]+=atof(pch);
          pch=__strtok_r(NULL," ,\t\n",&state);
          jind++;
        }
        iind++;
      }
      fclose(TR);
    }
    char colorout[15]; sprintf(colorout, "color_%.1f.%i",kk,RUNID);
    FILE *Fout;Fout = fopen(colorout,"w");
    fprintf(Fout,"%i %i %i\n",NMODE,NTIME,NPROP);
    for (i=0;i<rows;i++)
    {
      for (j=0;j<columns;j++) 
        if (j==0) fprintf(Fout,"%i ",(int)(DUMP[i][j]/ndir));
        else fprintf(Fout,"%e ",DUMP[i][j]/ndir);
      fprintf(Fout,"\n");
    }
    fclose(Fout);
    
    char modeout[14];sprintf(modeout,"MODEINFO_%.1f.%i",kk,RUNID);
    Fout=fopen(modeout,"w");
    for (i=0;i<NMODE;i++)
    {
      for(j=0;j<NPROP;j++)fprintf(Fout,"%e ",MINFO[i][j]/ndir);
      fprintf(Fout,"\n");
    }
    for(i=0;i<rows;i++)free(DUMP[i]);free(DUMP);
    for(i=0;i<NMODE;i++)free(MINFO[i]);free(MINFO);

    sprintf(cmd,"gzip %s",colorout); TR=popen(cmd,"r"); pclose(TR);

  }
  free(dir_name);
  char outdir[]="mean_color";
  sprintf(cmd,"rm -rf %s;mkdir %s;mv MODEINF* color_* %s;",outdir,outdir,outdir);
  TR=popen(cmd,"r");pclose(TR);
}
