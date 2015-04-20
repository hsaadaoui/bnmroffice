//---------------------------------------------------------------------
//------------------------Adapted from mud_util------------------------
//-----------------Modified for BNMR-GUI by H. Saadaoui----------------
//---------------------Last Modification: Jul-28-2007------------------
//Functions: reading data, headers and independent variables-----------
//Output: writes ASCII files or constuction and passes them to the GUI-

//---------------------------------C headers---------------------------
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//---------------------------------MUD Headers--------------------------
#include"mud.h"
//---------------------------------size of each variable----------------
#define size 1000

//Check for valid list of chosen histograms.  Return FALSE for invalid,
//TRUE for valid.  Also adjusts mhists to reflect number selected.
#define MUD_MAX_FILES 16
#define MUD_FILE_READ  1
#define MUD_FILE_WRITE 2

static FILE* mud_f[MUD_MAX_FILES] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static MUD_SEC_GRP* pMUD_fileGrp[MUD_MAX_FILES];

#define _strncpy( To, From, Len) strncpy( To, From, Len )[Len-1]='\0'

BOOL validateHistList ( nhsel, hsel, pmhists )int nhsel;  int hsel[];int* pmhists;
{
  int i,j;
  if ( nhsel > 0 ) 
  {   /* If specific histograms are selected */
      /* check the number selected is reasonable */
      if ( nhsel > 16 || nhsel > *pmhists )  return( FALSE );
      /* check that each selection is in range, and not duplicated */
      for ( i=0; i<nhsel; i++ )
      {
          if ( hsel[i] < 1 || hsel[i] > *pmhists ) return( FALSE );
          if ( i > 0 ) 
          {
              for ( j=0; j<i; j++ )
              {
                  if ( hsel[i] == hsel[j] ) return( FALSE );
              }
          }
      }
      *pmhists = nhsel;
  }
  return( TRUE );
}



int MUD2COL_convert( char*  fin_name,char* fout_name, int nhsel, int hsel[],
		     int * num_of_hists)
{
    FILE* fin = NULL;
    FILE* fout = NULL;
    MUD_SEC_GRP* pMUD_fileGrp = NULL;
    MUD_SEC_GRP* pMUD_histGrp = NULL;
    MUD_SEC_GEN_HIST_HDR* pMUD_histHdr;
    MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
    int num_hists;
    int i, j, n;
    UINT32*  pData[32] = { NULL };
    UINT32 nbMin = 2000000;
    char* p;
    
    /*     *  Read the file*/
    fin = MUD_openInput( fin_name );
    if( fin == NULL ) 
      {
	fprintf( stderr, "failed to open file \"%s\"\n", fin_name );
	goto error;
      }
    
    pMUD_fileGrp = MUD_readFile( fin );
    if( pMUD_fileGrp == NULL )
      {
	fprintf( stderr, "error while reading file %s\n", fin_name );
	goto error;
      }

    fclose( fin );
    
    
   
    if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TD_ID )
      {
	
	pMUD_histGrp = MUD_search( pMUD_fileGrp->pMem, 
				   MUD_SEC_GRP_ID, MUD_GRP_TRI_TD_HIST_ID,
				   (UINT32)0 );
      }
    else if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TI_ID )
      {
	
	pMUD_histGrp = MUD_search( pMUD_fileGrp->pMem, 
				   MUD_SEC_GRP_ID, MUD_GRP_TRI_TI_HIST_ID,
				   (UINT32)0 );
      }
    else
      {
	fprintf( stderr, "error: wrong format ID\n" );
	goto error;
      }
    
    
    if( pMUD_histGrp == NULL )
      {
	fprintf( stderr, "error: bad file format: histogram group not found\n" );
	goto error;
      }
    
    num_hists = pMUD_histGrp->num/2;  
    
    if ( ! validateHistList( nhsel, hsel, &num_hists ) )
      {
	printf( "warning: bad hist list -- ignored\n" );
	nhsel = 0;
      }
    
    fout = fopen( fout_name, "w" );
    if( fout == NULL )
      {
	fprintf( stderr, "failed to fopen output file \"%s\"\n", fout_name );
	goto error;
      }
    for( i = 0; i < num_hists; i++ )
      {
	if( nhsel > 0 )
	  n = hsel[i];
	else
	  n = i+1;
	/* *  Find the histogram header*/
	pMUD_histHdr = MUD_search( pMUD_histGrp->pMem, 
				   MUD_SEC_GEN_HIST_HDR_ID, (UINT32)(n), 
				   (UINT32)0 );
	//	*Histsn[i] = pMUD_histHdr->title;
	if( pMUD_histHdr == NULL ) 
	  {
	    printf( "error: bad file format: histogram header #%d not found", n );
	    goto error;
	  }
#ifdef DEBUG
	printf( "found hist hdr %d (%p)\n", n, pMUD_histHdr );
#endif /* DEBUG */
	
	/* *  Find the histogram data */
	pMUD_histDat = MUD_search( pMUD_histGrp->pMem, 
				   MUD_SEC_GEN_HIST_DAT_ID, (UINT32)(n), 
				   (UINT32)0 );
	if( pMUD_histDat == NULL ) 
	  {
	    printf( "error: bad file format: histogram data #%d not found", n );
	    goto error;
	  }
	
#ifdef DEBUG
	printf( "found hist data %d (%p)\n", n, pMUD_histDat );
#endif /* DEBUG */
	
	/* *  Allocate memory for the unpacked histogram*/
	pData[i] = (UINT32*)malloc( 4*pMUD_histHdr->nBins );
	if( nbMin > pMUD_histHdr->nBins ) nbMin = pMUD_histHdr->nBins;
	/*
	 *  Unpack the histogram
	 */
	MUD_SEC_GEN_HIST_unpack( pMUD_histHdr->nBins, 
				 pMUD_histHdr->bytesPerBin, pMUD_histDat->pData, 
				 4, pData[i] );
      }
    for( j = 0; j < nbMin; j++ )
      {
	for( i = 0; i < num_hists; i++ )
	  {
	    fprintf( fout, "%u ", pData[i][j] );
	    
	  }
	fprintf( fout, "\n" );
      }
    fclose( fout );
    for( i = 0; i < num_hists; i++ )
      {
	if( pData[i] != NULL ) free( pData[i] );
      }
    MUD_SEC_GEN_RUN_DESC* pMUD_desc;
    if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TD_ID )
      {
	pMUD_desc = MUD_search( pMUD_fileGrp->pMem, 
				MUD_SEC_GEN_RUN_DESC_ID/*MUD_SEC_TRI_TI_RUN_DESC_ID*/, (UINT32)1, 
				(UINT32)0 );
      }
    else if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TI_ID )
      {	
	pMUD_desc = MUD_search( pMUD_fileGrp->pMem, 
				MUD_SEC_TRI_TI_RUN_DESC_ID, (UINT32)1, 
				(UINT32)0 );
      }
    if( pMUD_desc == NULL )
      {
	printf( "error - bad file format, Run Description not found\n" );
	MUD_free( pMUD_fileGrp );
	return( FAILURE );
      }
    
    *num_of_hists      =    num_hists;
    
    MUD_free( pMUD_fileGrp );   
    return( SUCCESS );
 error:
    if( fin != NULL ) fclose( fin );
    if( fout != NULL ) fclose( fout );
    MUD_free( pMUD_fileGrp );
    for( i = 0; i < num_hists; i++ )
      {
	if( pData[i] != NULL ) free( pData[i] );
      }
    return( FAILURE );
}

struct runDescription {
  UINT32 ExptNumber;
  UINT32 RunNumber;
  //TIME   TimeBegin;
  //TIME   TimeEnd;
  UINT32 ElapsedSec;
  char	 Title[size];
  char	 Lab[size];
  char	 Area[size];
  char	 Method[size];
  char	 Apparatus[size];
  char	 Insert[size];
  char	 Sample[size];
  char	 Orient[size];
  char	 Das[size];
  char	 Experimenter[size];
  char	 Temp[size];
  char	 Field[size];
  int    Nb_ind_vrbls;
  char   Name[size][size];
  double Mean[size];
  double StdDev[size];
};

typedef struct runDescription runDescription;
runDescription rundesc(char * fin_name[])
{
  //printf("debuging 1\n");
  runDescription DESC;
  DESC.Nb_ind_vrbls = 0;
  
  /*  Read the file */
  MUD_SEC_GRP* pMUD_fileGrp = NULL;
  FILE* fin = NULL;
  fin = MUD_openInput( fin_name );
  if( fin == NULL ) 
    {
      fprintf( stderr, "\"%s\" could not be found\n", fin_name );
      fclose( fin );
      goto end;
    }
  
  pMUD_fileGrp = MUD_readFile( fin );
  if( pMUD_fileGrp == NULL )
    {
      MUD_free(pMUD_fileGrp);
      fprintf( stderr, "Error while reading %s\n", fin_name );
      goto end;
    }
  
  fclose( fin );
  //---------------------------------------------------------------------------
  MUD_SEC_GEN_RUN_DESC* pMUD_desc;
  if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TD_ID )
    {
      pMUD_desc = MUD_search( pMUD_fileGrp->pMem, 
			      MUD_SEC_GEN_RUN_DESC_ID, (UINT32)1, 
			      (UINT32)0 );
    }
  
  else if( MUD_instanceID( pMUD_fileGrp ) == MUD_FMT_TRI_TI_ID )
    {	
      pMUD_desc = MUD_search( pMUD_fileGrp->pMem, 
			      MUD_SEC_TRI_TI_RUN_DESC_ID, (UINT32)1, 
			      (UINT32)0 );
    }
  
  if( pMUD_desc == NULL )
    {
      printf( "No description in %s\n",fin_name );
      MUD_free( pMUD_fileGrp );
      goto end;
    }
  
  DESC.RunNumber  = pMUD_desc -> runNumber;
  DESC.ExptNumber = pMUD_desc -> exptNumber;
  //DESC.TimeBegin  = pMUD_desc -> timeBegin;// printf("exp=%d\n",time_0);
  //DESC.TimeEnd    = pMUD_desc -> timeEnd; //printf("exp=%d\n",time_f);
  DESC.ElapsedSec = pMUD_desc->elapsedSec;
  strcpy(DESC.Title, pMUD_desc -> title); 
  strcpy(DESC.Lab , pMUD_desc -> lab); 
  strcpy(DESC.Area, pMUD_desc -> area);
  strcpy(DESC.Method, pMUD_desc -> method); 
  strcpy(DESC.Apparatus, pMUD_desc -> apparatus);
  strcpy(DESC.Insert, pMUD_desc -> insert); 
  strcpy(DESC.Sample, pMUD_desc -> sample);
  strcpy(DESC.Orient, pMUD_desc -> orient);
  strcpy(DESC.Das, pMUD_desc -> das);
  strcpy(DESC.Experimenter, pMUD_desc -> experimenter);
  strcpy(DESC.Temp, pMUD_desc -> temperature); 
  strcpy(DESC.Field, pMUD_desc -> field);
  
  int    fh, index = 0, strdim = 1000;
  UINT32 pType,pNum;
  double mean,stddev;
  char  name[1000];


  
  fh  =  MUD_openRead(fin_name,&pType);
  int indvars = MUD_getIndVars(fh,&pType,&pNum );



  
  int nmax = pNum;
  if ( indvars == 0 )
    {
      //printf("Run %s has no indepdent variables\n",fin_name);
      nmax = 0;
      DESC.Nb_ind_vrbls  = 0;
      MUD_closeRead(fh);
      goto end;
    }
  else
    {	
      DESC.Nb_ind_vrbls  = nmax ;
      for (index = 0; index <nmax; index ++)
	{ 
	  MUD_getIndVarMean(fh,index+1,&mean);  
	  MUD_getIndVarStddev(fh,index+1,&stddev); 
	  MUD_getIndVarName(fh,index+1,name,strdim);
	  strcpy(DESC.Name[index],name);
	  DESC.Mean[index]       =  mean;
	  DESC.StdDev[index]     =  stddev;
	}
	MUD_closeRead(fh);
    }
  
 end:;
  MUD_free(pMUD_fileGrp);
  return( DESC );
}


int MUD_indVars(char * filename[],double varMean,char * varName[])
{
  int fh;
  UINT32 pType;
  //fh = MUD_openReadWrite(filename, &pType);
  fh = MUD_openReadWrite(filename, &pType); 
  UINT32 num=0;
  int indvars = MUD_getIndVars(fh,&pType,&num ); 
  MUD_setIndVars(  fh, pType,0);
  MUD_setIndVarLow(  fh,num,0.00);
  MUD_setIndVarHigh(  fh,num, 0.00 );
  MUD_setIndVarMean(  fh,num, varMean );
  MUD_setIndVarStddev(  fh,num, 0.0 );
  MUD_setIndVarSkewness(  fh,num, 0.0 );
  MUD_setIndVarName(  fh,num,varName);
  MUD_setIndVarDescription(  fh,num,"Tesla");
  MUD_setIndVarUnits(  fh,num, "test of program" );
  MUD_closeWrite(fh); 
  return( 1 );
}
  

int 
MUD_openRead( filename, pType )
    char* filename;
    UINT32* pType;
{
  int fd;
  FILE* fin;

  for( fd = 0; fd < MUD_MAX_FILES; fd++ ) 
  {
    if( mud_f[fd] == NULL ) break;
  }
  if( fd == MUD_MAX_FILES ) return( -1 );

  mud_f[fd] = MUD_openInput( filename );
  if( mud_f[fd] == NULL ) return( -1 );

  /*
   *  Just read the whole file
   *  Might want to make this more complicated, 
   *  i.e., only read when needed, keep track of 
   *  what's already read.
   */
  pMUD_fileGrp[fd] = MUD_readFile( mud_f[fd] );
  if( pMUD_fileGrp[fd] == NULL ) 
  {
    fclose( mud_f[fd] );
    mud_f[fd] = NULL;
    return( -1 );
  }

  *pType = MUD_instanceID( pMUD_fileGrp[fd] );

  return( fd );
}
/* This is the same as MUD_openInput except for access mode 
FILE*
MUD_openInOut( inFile )
    char* inFile;
{
    FILE* fin;

    fin = fopen( inFile, "r+b" );
    if( fin == NULL )
    {
	return( NULL );
    }
    return( fin );
}
*/
int 
MUD_openReadWrite( filename, pType )
    char* filename;
    UINT32* pType;
{
  int fd;
  FILE* fin;

  for( fd = 0; fd < MUD_MAX_FILES; fd++ ) 
  {
    if( mud_f[fd] == NULL ) break;
  }
  if( fd == MUD_MAX_FILES ) return( -1 );

  mud_f[fd] = MUD_openInOut( filename );
  if( mud_f[fd] == NULL ) return( -1 );

  /*
   *  Just read the whole file.  Must do so in ReadWrite version.
   */
  pMUD_fileGrp[fd] = MUD_readFile( mud_f[fd] );
  if( pMUD_fileGrp[fd] == NULL ) 
  {
    fclose( mud_f[fd] );
    mud_f[fd] = NULL;
    return( -1 );
  }

  *pType = MUD_instanceID( pMUD_fileGrp[fd] );

  return( fd );
}


int 
MUD_openWrite( filename, type )
    char* filename;
    UINT32 type;
{
  int fd;
  FILE* fin;

  for( fd = 0; fd < MUD_MAX_FILES; fd++ ) 
  {
    if( mud_f[fd] == NULL ) break;
  }
  if( fd == MUD_MAX_FILES ) return( -1 );

  mud_f[fd] = MUD_openOutput( filename );
  if( mud_f[fd] == NULL ) return( -1 );

  pMUD_fileGrp[fd] = (MUD_SEC_GRP*)MUD_new( MUD_SEC_GRP_ID, type );
  if( pMUD_fileGrp[fd] == NULL )
  {
    fclose( mud_f[fd] );
    mud_f[fd] = NULL;
    return( -1 );
  }

  return( fd );
}


int 
MUD_closeRead( fd )
    int fd;
{
  if( ( fd < 0 ) || ( fd >= MUD_MAX_FILES ) || ( mud_f[fd] == NULL ) ) 
  {
    return( 0 );
  }

  /*
   *  Free the list
   */
  if( pMUD_fileGrp[fd] != NULL )
  {
    MUD_free( pMUD_fileGrp[fd] );
    pMUD_fileGrp[fd] = NULL;
  }

  fclose( mud_f[fd] );
  mud_f[fd] = NULL;

  return( 1 );
}


int 
MUD_closeWrite( fd )
    int fd;
{
  if( ( fd < 0 ) || ( fd >= MUD_MAX_FILES ) || ( mud_f[fd] == NULL ) ) 
  {
    return( 0 );
  }

  /*
   *  Re-index mud groups (memSize and index.offset)
   */
  MUD_setSizes( pMUD_fileGrp[fd] );
  /*
   *  Write the file out if it's an output file
   */
  MUD_writeFile( mud_f[fd], pMUD_fileGrp[fd] ); 

  /*
   *  Free the list
   */
  if( pMUD_fileGrp[fd] != NULL )
  {
    MUD_free( pMUD_fileGrp[fd] );
    pMUD_fileGrp[fd] = NULL;
  }

  fclose( mud_f[fd] );
  mud_f[fd] = NULL;

  return( 1 );
}

int 
MUD_closeWriteFile( fd, outname )
    int fd;
    char* outname;
{
  if( ( fd < 0 ) || ( fd >= MUD_MAX_FILES ) || ( mud_f[fd] == NULL ) ) 
  {
    return( 0 );
  }

  /*
   * Close the input file
   */
  fclose( mud_f[fd] );

  /*
   * Open output file on same fd index
   */
  mud_f[fd] = MUD_openOutput( outname );
  if( mud_f[fd] == NULL ) return( 0 );

  /*
   *  Re-index mud groups (memSize and index.offset)
   */
  MUD_setSizes( pMUD_fileGrp[fd] );
  /*
   *  Write the file out.
   */
  MUD_writeFile( mud_f[fd], pMUD_fileGrp[fd] ); 

  /*
   *  Free the list
   */
  if( pMUD_fileGrp[fd] != NULL )
  {
    MUD_free( pMUD_fileGrp[fd] );
    pMUD_fileGrp[fd] = NULL;
  }

  fclose( mud_f[fd] );
  mud_f[fd] = NULL;

  return( 1 );
}


#define _check_fd( fd )  if( ( fd < 0 ) || \
                             ( fd >= MUD_MAX_FILES ) || \
                             ( mud_f[fd] == NULL ) ) return( 0 )

/*
 *  Run Description
 */
#define _sea_desc( fd ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
      pMUD_idesc = MUD_search( pMUD_fileGrp[fd]->pMem, \
                              MUD_SEC_TRI_TI_RUN_DESC_ID, (UINT32)1, \
                              (UINT32)0 ); \
      if( pMUD_idesc == NULL ) return( 0 ); \
      break; \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_desc = MUD_search( pMUD_fileGrp[fd]->pMem, \
                              MUD_SEC_GEN_RUN_DESC_ID, (UINT32)1, \
                              (UINT32)0 ); \
      if( pMUD_desc == NULL ) return( 0 ); \
      break; \
  }


#define _sea_gdesc( fd ) \
  pMUD_desc = MUD_search( pMUD_fileGrp[fd]->pMem, \
                              MUD_SEC_GEN_RUN_DESC_ID, (UINT32)1, \
                              (UINT32)0 ); \
  if( pMUD_desc == NULL ) return( 0 )


#define _sea_idesc( fd ) \
  pMUD_idesc = MUD_search( pMUD_fileGrp[fd]->pMem, \
                              MUD_SEC_TRI_TI_RUN_DESC_ID, (UINT32)1, \
                              (UINT32)0 ); \
  if( pMUD_idesc == NULL ) return( 0 )


#define _desc_uint_getproc( name, var ) \
int name( fd, var ) \
    int fd; \
    UINT32* var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_desc( fd ); \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: *var = pMUD_idesc->var; break; \
    case MUD_FMT_TRI_TD_ID: default: *var = pMUD_desc->var; break; \
  } \
  return( 1 ); \
}


#define _desc_uint_setproc( name, var ) \
int name( fd, var ) \
    int fd; \
    UINT32 var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_desc( fd ); \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: pMUD_idesc->var = var; break; \
    case MUD_FMT_TRI_TD_ID: default: pMUD_desc->var = var; break; \
  } \
  return( 1 ); \
}


#define _desc_char_getproc( name, var ) \
int name( fd, var, strdim ) \
    int fd, strdim; \
    char* var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_desc( fd ); \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: _strncpy( var, pMUD_idesc->var, strdim ); break; \
    case MUD_FMT_TRI_TD_ID: default: _strncpy( var, pMUD_desc->var, strdim ); break; \
  } \
  return( 1 ); \
}


#define _desc_char_setproc( name, var ) \
int name( fd, var ) \
    int fd; \
    char* var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_desc( fd ); \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
      _free( pMUD_idesc->var ); pMUD_idesc->var = strdup( var ); break; \
    case MUD_FMT_TRI_TD_ID: default: \
      _free( pMUD_desc->var ); pMUD_desc->var = strdup( var ); break; \
  } \
  return( 1 ); \
}


#define _gdesc_char_getproc( name, var ) \
int name( fd, var, strdim ) \
    int fd, strdim; \
    char* var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  _check_fd( fd ); \
  _sea_gdesc( fd ); \
  _strncpy( var, pMUD_desc->var, strdim ); \
  return( 1 ); \
}


#define _gdesc_char_setproc( name, var ) \
int name( fd, var ) \
    int fd; \
    char* var; \
{ \
  MUD_SEC_GEN_RUN_DESC* pMUD_desc; \
  _check_fd( fd ); \
  _sea_gdesc( fd ); \
  _free( pMUD_desc->var ); \
  pMUD_desc->var = strdup( var ); \
  return( 1 ); \
}


#define _idesc_char_getproc( name, var ) \
int name( fd, var, strdim ) \
    int fd, strdim;\
    char* var;\
{ \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_idesc( fd ); \
  _strncpy( var, pMUD_idesc->var, strdim ); \
  return( 1 ); \
}


#define _idesc_char_setproc( name, var ) \
int name( fd, var ) \
    int fd;\
    char* var;\
{ \
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc; \
  _check_fd( fd ); \
  _sea_idesc( fd ); \
  _free( pMUD_idesc->var ); \
  pMUD_idesc->var = strdup( var ); \
  return( 1 ); \
}


int
MUD_getRunDesc( fd, pType )
    int fd;
    UINT32* pType;
{
  MUD_SEC_GEN_RUN_DESC* pMUD_desc;
  MUD_SEC_TRI_TI_RUN_DESC* pMUD_idesc;

  _check_fd( fd );

  switch( MUD_instanceID( pMUD_fileGrp[fd] ) )
  {
    case MUD_FMT_TRI_TI_ID:
      pMUD_idesc = MUD_search( pMUD_fileGrp[fd]->pMem,
                              MUD_SEC_TRI_TI_RUN_DESC_ID, (UINT32)1,
                              (UINT32)0 );
      if( pMUD_idesc == NULL ) return( 0 );
      *pType = MUD_SEC_TRI_TI_RUN_DESC_ID;
      break;
    case MUD_FMT_TRI_TD_ID:
    default:
      pMUD_desc = MUD_search( pMUD_fileGrp[fd]->pMem,
                              MUD_SEC_GEN_RUN_DESC_ID, (UINT32)1,
                              (UINT32)0 );
      if( pMUD_desc == NULL ) return( 0 );
      *pType = MUD_SEC_GEN_RUN_DESC_ID;
      break;
  }

  return( 1 );
}

int
MUD_setRunDesc( fd, type )
    int fd;
    UINT32 type;
{
  void* pMUD_desc;

  _check_fd( fd );

  switch( MUD_instanceID( pMUD_fileGrp[fd] ) )
  {
    case MUD_FMT_TRI_TI_ID:
      pMUD_desc = (void*)MUD_new( MUD_SEC_TRI_TI_RUN_DESC_ID, 1 );
      if( pMUD_desc == NULL ) return( 0 );
      break;
    case MUD_FMT_TRI_TD_ID:
    default:
      pMUD_desc = (void*)MUD_new( MUD_SEC_GEN_RUN_DESC_ID, 1 );
      if( pMUD_desc == NULL ) return( 0 );
      break;
  }

  MUD_addToGroup( pMUD_fileGrp[fd], pMUD_desc );

  return( 1 );
}

_desc_uint_getproc( MUD_getExptNumber, exptNumber )
_desc_uint_getproc( MUD_getRunNumber, runNumber )
_desc_uint_getproc( MUD_getElapsedSec, elapsedSec )
_desc_uint_getproc( MUD_getTimeBegin, timeBegin )
_desc_uint_getproc( MUD_getTimeEnd, timeEnd )
_desc_char_getproc( MUD_getTitle, title )
_desc_char_getproc( MUD_getLab, lab )
_desc_char_getproc( MUD_getArea, area )
_desc_char_getproc( MUD_getMethod, method )
_desc_char_getproc( MUD_getApparatus, apparatus )
_desc_char_getproc( MUD_getInsert, insert )
_desc_char_getproc( MUD_getSample, sample )
_desc_char_getproc( MUD_getOrient, orient )
_desc_char_getproc( MUD_getDas, das )
_desc_char_getproc( MUD_getExperimenter, experimenter )
/* not in TRI_TI */
_gdesc_char_getproc( MUD_getTemperature, temperature )
_gdesc_char_getproc( MUD_getField, field )
/* TRI_TI only */
_idesc_char_getproc( MUD_getSubtitle, subtitle )
_idesc_char_getproc( MUD_getComment1, comment1 )
_idesc_char_getproc( MUD_getComment2, comment2 )
_idesc_char_getproc( MUD_getComment3, comment3 )

_desc_uint_setproc( MUD_setExptNumber, exptNumber )
_desc_uint_setproc( MUD_setRunNumber, runNumber )
_desc_uint_setproc( MUD_setElapsedSec, elapsedSec )
_desc_uint_setproc( MUD_setTimeBegin, timeBegin )
_desc_uint_setproc( MUD_setTimeEnd, timeEnd )
_desc_char_setproc( MUD_setTitle, title )
_desc_char_setproc( MUD_setLab, lab )
_desc_char_setproc( MUD_setArea, area )
_desc_char_setproc( MUD_setMethod, method )
_desc_char_setproc( MUD_setApparatus, apparatus )
_desc_char_setproc( MUD_setInsert, insert )
_desc_char_setproc( MUD_setSample, sample )
_desc_char_setproc( MUD_setOrient, orient )
_desc_char_setproc( MUD_setDas, das )
_desc_char_setproc( MUD_setExperimenter, experimenter )
/* not in TRI_TI */
_gdesc_char_setproc( MUD_setTemperature, temperature )
_gdesc_char_setproc( MUD_setField, field )
/* TRI_TI only */
_idesc_char_setproc( MUD_setSubtitle, subtitle )
_idesc_char_setproc( MUD_setComment1, comment1 )
_idesc_char_setproc( MUD_setComment2, comment2 )
_idesc_char_setproc( MUD_setComment3, comment3 )


/*
 *  Comments
 */
#define _sea_cmtgrp( fd ) \
  pMUD_cmtGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                          MUD_GRP_CMT_ID, (UINT32)1, \
                          (UINT32)0 ); \
  if( pMUD_cmtGrp == NULL ) return( 0 )


#define _sea_cmt( fd, n ) \
  pMUD_cmt = MUD_search( pMUD_cmtGrp->pMem, \
                         MUD_SEC_CMT_ID, (UINT32)n, \
                         (UINT32)0 ); \
  if( pMUD_cmt == NULL ) return( 0 )


#define _cmt_uint_getproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    UINT32* var;\
{ \
  MUD_SEC_GRP* pMUD_cmtGrp; \
  MUD_SEC_CMT* pMUD_cmt; \
  _check_fd( fd ); \
  _sea_cmtgrp( fd ); \
  _sea_cmt( fd, num ); \
  *var = pMUD_cmt->var; \
  return( 1 ); \
}


#define _cmt_uint_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    UINT32 var;\
{ \
  MUD_SEC_GRP* pMUD_cmtGrp; \
  MUD_SEC_CMT* pMUD_cmt; \
  _check_fd( fd ); \
  _sea_cmtgrp( fd ); \
  _sea_cmt( fd, num ); \
  pMUD_cmt->var = var; \
  return( 1 ); \
}


#define _cmt_char_getproc( name, var ) \
int \
name( fd, num, var, strdim ) \
    int fd, strdim;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_cmtGrp; \
  MUD_SEC_CMT* pMUD_cmt; \
  _check_fd( fd ); \
  _sea_cmtgrp( fd ); \
  _sea_cmt( fd, num ); \
  _strncpy( var, pMUD_cmt->var, strdim ); \
  return( 1 ); \
}

#define _cmt_char_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_cmtGrp; \
  MUD_SEC_CMT* pMUD_cmt; \
  _check_fd( fd ); \
  _sea_cmtgrp( fd ); \
  _sea_cmt( fd, num ); \
  _free( pMUD_cmt->var ); \
  pMUD_cmt->var = strdup( var ); \
  return( 1 ); \
}

int 
MUD_getComments( fd, pType, pNum )
    int fd;
    UINT32* pType;
    UINT32* pNum;
{
  MUD_SEC_GRP* pMUD_cmtGrp;

  _check_fd( fd );
  _sea_cmtgrp( fd );
  *pType = MUD_instanceID( pMUD_cmtGrp );
  *pNum = pMUD_cmtGrp->num;

  return( 1 );
}

int 
MUD_setComments( fd, type, num )
    int fd;
    UINT32 type;
    UINT32 num;
{
  MUD_SEC_GRP* pMUD_cmtGrp;
  MUD_SEC_CMT* pMUD_cmt;
  int i;

  _check_fd( fd );

  pMUD_cmtGrp = (MUD_SEC_GRP*)MUD_new( MUD_SEC_GRP_ID, type );
  if( pMUD_cmtGrp == NULL ) return( 0 );

  for( i = 0; i < num; i++ )
  {
    pMUD_cmt = (MUD_SEC_CMT*)MUD_new( MUD_SEC_CMT_ID, i+1 );
    if( pMUD_cmt == NULL )
    {
      MUD_free( pMUD_cmtGrp );
      return( 0 );
    }
    MUD_addToGroup( pMUD_cmtGrp, pMUD_cmt );
  }

  MUD_addToGroup( pMUD_fileGrp[fd], pMUD_cmtGrp );

  return( 1 );
}

_cmt_uint_getproc( MUD_getCommentPrev, prevReplyID )
_cmt_uint_getproc( MUD_getCommentNext, nextReplyID )
_cmt_uint_getproc( MUD_getCommentTime, time )
_cmt_char_getproc( MUD_getCommentAuthor, author )
_cmt_char_getproc( MUD_getCommentTitle, title )
_cmt_char_getproc( MUD_getCommentBody, comment )

_cmt_uint_setproc( MUD_setCommentPrev, prevReplyID )
_cmt_uint_setproc( MUD_setCommentNext, nextReplyID )
_cmt_uint_setproc( MUD_setCommentTime, time )
_cmt_char_setproc( MUD_setCommentAuthor, author )
_cmt_char_setproc( MUD_setCommentTitle, title )
_cmt_char_setproc( MUD_setCommentBody, comment )


/*
 *  Histograms
 */
#define _sea_histgrp( fd ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
      pMUD_histGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                                 MUD_SEC_GRP_ID, MUD_GRP_TRI_TI_HIST_ID, \
                                 (UINT32)0 ); \
      break; \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_histGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                                 MUD_SEC_GRP_ID, MUD_GRP_TRI_TD_HIST_ID, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_histGrp == NULL ) return( 0 )


#define _sea_histhdr( fd, n ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_histHdr = MUD_search( pMUD_histGrp->pMem, \
                                 MUD_SEC_GEN_HIST_HDR_ID, (UINT32)n, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_histHdr == NULL ) return( 0 )


#define _hist_uint_getproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    UINT32* var;\
{ \
  MUD_SEC_GRP* pMUD_histGrp; \
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr; \
  _check_fd( fd ); \
  _sea_histgrp( fd ); \
  _sea_histhdr( fd, num ); \
  *var = pMUD_histHdr->var; \
  return( 1 ); \
}

#define _hist_uint_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    UINT32 var;\
{ \
  MUD_SEC_GRP* pMUD_histGrp; \
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr; \
  _check_fd( fd ); \
  _sea_histgrp( fd ); \
  _sea_histhdr( fd, num ); \
  pMUD_histHdr->var = var; \
  return( 1 ); \
}

#define _hist_char_getproc( name, var ) \
int \
name( fd, num, var, strdim ) \
    int fd, strdim;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_histGrp; \
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr; \
  _check_fd( fd ); \
  _sea_histgrp( fd ); \
  _sea_histhdr( fd, num ); \
  _strncpy( var, pMUD_histHdr->var, strdim ); \
  return( 1 ); \
}

#define _hist_char_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_histGrp; \
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr; \
  _check_fd( fd ); \
  _sea_histgrp( fd ); \
  _sea_histhdr( fd, num ); \
  _free( pMUD_histHdr->var ); \
  pMUD_histHdr->var = strdup( var ); \
  return( 1 ); \
}

int 
MUD_getHists( fd, pType, pNum )
    int fd;
    UINT32* pType;
    UINT32* pNum;
{
  MUD_SEC_GRP* pMUD_histGrp;
  _check_fd( fd );
  _sea_histgrp( fd );
  *pType = MUD_instanceID( pMUD_histGrp );
  switch( *pType )
  {
    case MUD_GRP_TRI_TI_HIST_ID:
    case MUD_GRP_TRI_TD_HIST_ID:
    default:
      /* 
       *  NOTE: two sections per histogram (header, data)
       */
      *pNum = pMUD_histGrp->num/2;   
      break;
  }
  return( 1 );
}

int 
MUD_setHists( fd, type, num )
    int fd;
    UINT32 type;
    UINT32 num;
{
  MUD_SEC_GRP* pMUD_grp;
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr;
  MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
  int i;

  _check_fd( fd );

  pMUD_grp = (MUD_SEC_GRP*)MUD_new( MUD_SEC_GRP_ID, type );
  if( pMUD_grp == NULL ) return( 0 );

  for( i = 0; i < num; i++ )
  {
    pMUD_histHdr = (MUD_SEC_GEN_HIST_HDR*)MUD_new( MUD_SEC_GEN_HIST_HDR_ID, i+1 );
    if( pMUD_histHdr == NULL )
    {
      MUD_free( pMUD_grp );
      return( 0 );
    }
    MUD_addToGroup( pMUD_grp, pMUD_histHdr );
    pMUD_histDat = (MUD_SEC_GEN_HIST_DAT*)MUD_new( MUD_SEC_GEN_HIST_DAT_ID, i+1 );
    if( pMUD_histDat == NULL )
    {
      MUD_free( pMUD_grp );
      return( 0 );
    }
    MUD_addToGroup( pMUD_grp, pMUD_histDat );
  }

  MUD_addToGroup( pMUD_fileGrp[fd], pMUD_grp );

  return( 1 );
}

_hist_uint_getproc( MUD_getHistType, histType )
_hist_uint_getproc( MUD_getHistNumBytes, nBytes )
_hist_uint_getproc( MUD_getHistNumBins, nBins )
_hist_uint_getproc( MUD_getHistBytesPerBin, bytesPerBin )
_hist_uint_getproc( MUD_getHistFsPerBin, fsPerBin )
_hist_uint_getproc( MUD_getHistT0_Ps, t0_ps )
_hist_uint_getproc( MUD_getHistT0_Bin, t0_bin )
_hist_uint_getproc( MUD_getHistGoodBin1, goodBin1 )
_hist_uint_getproc( MUD_getHistGoodBin2, goodBin2 )
_hist_uint_getproc( MUD_getHistBkgd1, bkgd1 )
_hist_uint_getproc( MUD_getHistBkgd2, bkgd2 )
_hist_uint_getproc( MUD_getHistNumEvents, nEvents )
_hist_char_getproc( MUD_getHistTitle, title )

_hist_uint_setproc( MUD_setHistType, histType )
_hist_uint_setproc( MUD_setHistNumBytes, nBytes )
_hist_uint_setproc( MUD_setHistNumBins, nBins )
_hist_uint_setproc( MUD_setHistBytesPerBin, bytesPerBin )
_hist_uint_setproc( MUD_setHistFsPerBin, fsPerBin )
_hist_uint_setproc( MUD_setHistT0_Ps, t0_ps )
_hist_uint_setproc( MUD_setHistT0_Bin, t0_bin )
_hist_uint_setproc( MUD_setHistGoodBin1, goodBin1 )
_hist_uint_setproc( MUD_setHistGoodBin2, goodBin2 )
_hist_uint_setproc( MUD_setHistBkgd1, bkgd1 )
_hist_uint_setproc( MUD_setHistBkgd2, bkgd2 )
_hist_uint_setproc( MUD_setHistNumEvents, nEvents )
_hist_char_setproc( MUD_setHistTitle, title )

int 
MUD_getHistpData( fd, num, ppData )
    int fd;
    int num;
    void** ppData;
{
  MUD_SEC_GRP* pMUD_histGrp;
  MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
  _check_fd( fd );
  _sea_histgrp( fd );
  
  pMUD_histDat = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_DAT_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histDat == NULL ) return( 0 );

  *ppData = pMUD_histDat->pData;
  return( 1 );
}

int 
MUD_setHistpData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_histGrp;
  MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
  _check_fd( fd );
  _sea_histgrp( fd );
  
  pMUD_histDat = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_DAT_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histDat == NULL ) return( 0 );

  pMUD_histDat->pData = pData;
  return( 1 );
}

int 
MUD_getHistData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_histGrp;
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr;
  MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
  _check_fd( fd );
  _sea_histgrp( fd );
  
  pMUD_histHdr = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_HDR_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histHdr == NULL ) return( 0 );

  pMUD_histDat = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_DAT_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histDat == NULL ) return( 0 );

  /*
   *  Do unpacking/byte swapping
   */
  MUD_unpack( pMUD_histHdr->nBins, 
            pMUD_histHdr->bytesPerBin, pMUD_histDat->pData, 
    ( pMUD_histHdr->bytesPerBin == 0 ) ? 4 : pMUD_histHdr->bytesPerBin, pData );

  return( 1 );
}

int 
MUD_setHistData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_histGrp;
  MUD_SEC_GEN_HIST_HDR* pMUD_histHdr;
  MUD_SEC_GEN_HIST_DAT* pMUD_histDat;
  _check_fd( fd );
  _sea_histgrp( fd );
  
  pMUD_histHdr = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_HDR_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histHdr == NULL ) return( 0 );

  pMUD_histDat = MUD_search( pMUD_histGrp->pMem,
                             MUD_SEC_GEN_HIST_DAT_ID, (UINT32)num,
                             (UINT32)0 );
  if( pMUD_histDat == NULL ) return( 0 );

  _free( pMUD_histDat->pData );

  switch( pMUD_histHdr->bytesPerBin )
  {
    case 0:
      pMUD_histDat->pData = (caddr_t)zalloc( 4*pMUD_histHdr->nBins + 32 );
      break;
    default:
      pMUD_histDat->pData = (caddr_t)zalloc( 
                               pMUD_histHdr->nBins*pMUD_histHdr->bytesPerBin );
      break;
  }

  /*
   *  Do packing/byte swapping
   */
  pMUD_histDat->nBytes = pMUD_histHdr->nBytes = 
    MUD_pack( pMUD_histHdr->nBins, 
              ( pMUD_histHdr->bytesPerBin == 0 ) ? 4 : pMUD_histHdr->bytesPerBin, pData,
              pMUD_histHdr->bytesPerBin, pMUD_histDat->pData );

  return( 1 );
}

int 
MUD_getHistpTimeData( fd, num, ppTimeData )
    int fd;
    int num;
    UINT32** ppTimeData;
{
  /* return pointer to time history data for a histogram */
  /* not implemented */
  return( 1 );
}

int 
MUD_getHistTimeData( fd, num, pTimeData )
    int fd;
    int num;
    UINT32* pTimeData;
{
  /* return pointer to time history data for a histogram */
  /* not implemented */
  return( 1 );
}

int 
MUD_setHistpTimeData( fd, num, pTimeData )
    int fd;
    int num;
    UINT32* pTimeData;
{
  /* return pointer to time history data for a histogram */
  /* not implemented */
  return( 1 );
}

int 
MUD_setHistTimeData( fd, num, pTimeData )
    int fd;
    int num;
    UINT32* pTimeData;
{
  /* return pointer to time history data for a histogram */
  /* not implemented */
  return( 1 );
}

/*
 *  Returns number of bytes in outArray
 *  (not success/failure)
 */
int 
MUD_pack( num, inBinSize, inArray, outBinSize, outArray )
    int num;
    int inBinSize;
    void* inArray;
    int outBinSize;
    void* outArray;
{
  return( MUD_SEC_GEN_HIST_pack( num, inBinSize, inArray,
                                 outBinSize, outArray ) );
}


int 
MUD_unpack( num, inBinSize, inArray, outBinSize, outArray )
    int num;
    int inBinSize;
    void* inArray;
    int outBinSize;
    void* outArray;
{
  return( MUD_SEC_GEN_HIST_unpack( num, inBinSize, inArray,
                                   outBinSize, outArray ) );
}


/*
 *  Scalers
 */
#define _sea_scalgrp( fd ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_scalGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                                 MUD_SEC_GRP_ID, MUD_GRP_TRI_TD_SCALER_ID, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_scalGrp == NULL ) return( 0 )


#define _sea_scal( fd, n ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_scal = MUD_search( pMUD_scalGrp->pMem, \
                                 MUD_SEC_GEN_SCALER_ID, (UINT32)n, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_scal == NULL ) return( 0 )


int 
MUD_getScalers( fd, pType, pNum )
    int fd;
    UINT32* pType;
    UINT32* pNum;
{
  MUD_SEC_GRP* pMUD_scalGrp;

  _check_fd( fd );
  _sea_scalgrp( fd );
  *pType = MUD_instanceID( pMUD_scalGrp );
  *pNum = pMUD_scalGrp->num;

  return( 1 );
}

int 
MUD_setScalers( fd, type, num )
    int fd;
    UINT32 type;
    UINT32 num;
{
  MUD_SEC_GRP* pMUD_grp;
  MUD_SEC_GEN_SCALER* pMUD_scal;
  int i;

  _check_fd( fd );

  pMUD_grp = (MUD_SEC_GRP*)MUD_new( MUD_SEC_GRP_ID, type );
  if( pMUD_grp == NULL ) return( 0 );

  for( i = 0; i < num; i++ )
  {
    pMUD_scal = (MUD_SEC_GEN_SCALER*)MUD_new( MUD_SEC_GEN_SCALER_ID, i+1 );
    if( pMUD_scal == NULL )
    {
      MUD_free( pMUD_grp );
      return( 0 );
    }
    MUD_addToGroup( pMUD_grp, pMUD_scal );
  }

  MUD_addToGroup( pMUD_fileGrp[fd], pMUD_grp );

  return( 1 );
}

int 
MUD_getScalerLabel( fd, num, label, strdim )
    int fd, strdim;
    int num;
    char* label;
{
  MUD_SEC_GRP* pMUD_scalGrp;
  MUD_SEC_GEN_SCALER* pMUD_scal;

  _check_fd( fd );
  _sea_scalgrp( fd );
  _sea_scal( fd, num );
  _strncpy( label, pMUD_scal->label, strdim );

  return( 1 );
}

int 
MUD_setScalerLabel( fd, num, label )
    int fd;
    int num;
    char* label;
{
  MUD_SEC_GRP* pMUD_scalGrp;
  MUD_SEC_GEN_SCALER* pMUD_scal;

  _check_fd( fd );
  _sea_scalgrp( fd );
  _sea_scal( fd, num );
  _free( pMUD_scal->label );
  pMUD_scal->label = strdup( label );

  return( 1 );
}

int 
MUD_getScalerCounts( fd, num, pCounts )
    int fd;
    int num;
    UINT32* pCounts;
{
  MUD_SEC_GRP* pMUD_scalGrp;
  MUD_SEC_GEN_SCALER* pMUD_scal;

  _check_fd( fd );
  _sea_scalgrp( fd );
  _sea_scal( fd, num );

  pCounts[0] = pMUD_scal->counts[0];
  pCounts[1] = pMUD_scal->counts[1];

  return( 1 );
}

int 
MUD_setScalerCounts( fd, num, pCounts )
    int fd;
    int num;
    UINT32* pCounts;
{
  MUD_SEC_GRP* pMUD_scalGrp;
  MUD_SEC_GEN_SCALER* pMUD_scal;

  _check_fd( fd );
  _sea_scalgrp( fd );
  _sea_scal( fd, num );

  pMUD_scal->counts[0] = pCounts[0];
  pMUD_scal->counts[1] = pCounts[1];

  return( 1 );
}


/*
 *  Independent variables
 */
#define _sea_indvargrp( fd ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
      pMUD_indVarGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                                 MUD_SEC_GRP_ID, MUD_GRP_GEN_IND_VAR_ARR_ID, \
                                 (UINT32)0 ); \
      break; \
    case MUD_FMT_TRI_TD_ID: \
    default: \
      pMUD_indVarGrp = MUD_search( pMUD_fileGrp[fd]->pMem, \
                                 MUD_SEC_GRP_ID, MUD_GRP_GEN_IND_VAR_ID, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_indVarGrp == NULL ) return( 0 )


#define _sea_indvar( fd, n ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TD_ID: \
    case MUD_FMT_TRI_TI_ID: \
    default: \
      pMUD_indVar = MUD_search( pMUD_indVarGrp->pMem, \
                                 MUD_SEC_GEN_IND_VAR_ID, (UINT32)n, \
                                 (UINT32)0 ); \
      break; \
  } \
  if( pMUD_indVar == NULL ) return( 0 )


#define _sea_indvardat( fd, n ) \
  switch( MUD_instanceID( pMUD_fileGrp[fd] ) ) \
  { \
    case MUD_FMT_TRI_TI_ID: \
    default: \
      pMUD_array = MUD_search( pMUD_indVarGrp->pMem, \
                               MUD_SEC_GEN_ARRAY_ID, (UINT32)n, \
                               (UINT32)0 ); \
      break; \
  } \
  if( pMUD_array == NULL ) return( 0 )


#define _indvar_doub_getproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    double* var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_IND_VAR* pMUD_indVar; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvar( fd, num ); \
  *var = pMUD_indVar->var; \
  return( 1 ); \
}

#define _indvar_doub_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    double var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_IND_VAR* pMUD_indVar; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvar( fd, num ); \
  pMUD_indVar->var = var; \
  return( 1 ); \
}


#define _indvar_char_getproc( name, var ) \
int \
name( fd, num, var, strdim ) \
    int fd, strdim;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_IND_VAR* pMUD_indVar; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvar( fd, num ); \
  _strncpy( var, pMUD_indVar->var, strdim ); \
  return( 1 ); \
}

#define _indvar_char_setproc( name, var ) \
int \
name( fd, num, var ) \
    int fd;\
    int num;\
    char* var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_IND_VAR* pMUD_indVar; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvar( fd, num ); \
  _free( pMUD_indVar->var ); \
  pMUD_indVar->var = strdup( var ); \
  return( 1 ); \
}


#define _indvardat_uint_getproc( name, var ) \
int \
name( fd, n, var ) \
    int fd;\
    int n;\
    UINT32* var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_ARRAY* pMUD_array; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvardat( fd, n ); \
  *var = pMUD_array->var; \
  return( 1 ); \
}

#define _indvardat_uint_setproc( name, var ) \
int \
name( fd, n, var ) \
    int fd;\
    int n;\
    UINT32 var;\
{ \
  MUD_SEC_GRP* pMUD_indVarGrp; \
  MUD_SEC_GEN_ARRAY* pMUD_array; \
  _check_fd( fd ); \
  _sea_indvargrp( fd ); \
  _sea_indvardat( fd, n ); \
  pMUD_array->var = var; \
  return( 1 ); \
}


int 
MUD_getIndVars( fd, pType, pNum )
    int fd;
    UINT32* pType;
    UINT32* pNum;
{
  MUD_SEC_GRP* pMUD_indVarGrp;

  _check_fd( fd );
  _sea_indvargrp( fd );

  *pType = MUD_instanceID( pMUD_indVarGrp );
  switch( *pType )
  {
    case MUD_GRP_GEN_IND_VAR_ARR_ID:
      /*
       *  NOTE: two sections per independent variable (header,array)
       */
      *pNum = pMUD_indVarGrp->num/2;
      break;
    case MUD_GRP_GEN_IND_VAR_ID:
    default:
      *pNum = pMUD_indVarGrp->num;
      break;
  }
  return( 1 );
}

int 
MUD_setIndVars( fd, type, num )
    int fd;
    UINT32 type;
    UINT32 num;
{
  MUD_SEC_GRP* pMUD_grp;
  MUD_SEC_GEN_IND_VAR* pMUD_indVar;
  MUD_SEC_GEN_ARRAY* pMUD_array;
  int i;

  _check_fd( fd );

  pMUD_grp = (MUD_SEC_GRP*)MUD_new( MUD_SEC_GRP_ID, type );
  if( pMUD_grp == NULL ) return( 0 );

  for( i = 0; i < num; i++ )
  {
    pMUD_indVar = (MUD_SEC_GEN_IND_VAR*)MUD_new( MUD_SEC_GEN_IND_VAR_ID, i+1 );
    if( pMUD_indVar == NULL )
    {
      MUD_free( pMUD_grp );
      return( 0 );
    }
    MUD_addToGroup( pMUD_grp, pMUD_indVar );

    if( type == MUD_GRP_GEN_IND_VAR_ARR_ID )
    {
      pMUD_array = (MUD_SEC_GEN_ARRAY*)MUD_new( MUD_SEC_GEN_ARRAY_ID, i+1 );
      if( pMUD_array == NULL )
      {
        MUD_free( pMUD_grp );
        return( 0 );
      }
      MUD_addToGroup( pMUD_grp, pMUD_array );
    }
  }

  MUD_addToGroup( pMUD_fileGrp[fd], pMUD_grp );

  return( 1 );
}

_indvar_doub_getproc( MUD_getIndVarLow, low )
_indvar_doub_getproc( MUD_getIndVarHigh, high )
_indvar_doub_getproc( MUD_getIndVarMean, mean )
_indvar_doub_getproc( MUD_getIndVarStddev, stddev )
_indvar_doub_getproc( MUD_getIndVarSkewness, skewness )
_indvar_char_getproc( MUD_getIndVarName, name )
_indvar_char_getproc( MUD_getIndVarDescription, description )
_indvar_char_getproc( MUD_getIndVarUnits, units )

_indvardat_uint_getproc( MUD_getIndVarNumData, num )
_indvardat_uint_getproc( MUD_getIndVarElemSize, elemSize )
_indvardat_uint_getproc( MUD_getIndVarDataType, type )
_indvardat_uint_getproc( MUD_getIndVarHasTime, hasTime )

_indvar_doub_setproc( MUD_setIndVarLow, low )
_indvar_doub_setproc( MUD_setIndVarHigh, high )
_indvar_doub_setproc( MUD_setIndVarMean, mean )
_indvar_doub_setproc( MUD_setIndVarStddev, stddev )
_indvar_doub_setproc( MUD_setIndVarSkewness, skewness )
_indvar_char_setproc( MUD_setIndVarName, name )
_indvar_char_setproc( MUD_setIndVarDescription, description )
_indvar_char_setproc( MUD_setIndVarUnits, units )

_indvardat_uint_setproc( MUD_setIndVarNumData, num )
_indvardat_uint_setproc( MUD_setIndVarElemSize, elemSize )
_indvardat_uint_setproc( MUD_setIndVarDataType, type )

int
MUD_getIndVarpData( fd, num, ppData )
    int fd;
    int num;
    void** ppData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 
  *ppData = (void*)pMUD_array->pData;
  return( 1 ); 
}

int
MUD_setIndVarpData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 
  pMUD_array->pData = (caddr_t)pData;
  return( 1 ); 
}

int
MUD_getIndVarData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 

  switch( pMUD_array->type )
  {
    case 1:
      /*
       *  Do unpacking/byte swapping
       */
      MUD_unpack( pMUD_array->num, 
                  pMUD_array->elemSize, pMUD_array->pData, 
                  ( pMUD_array->elemSize == 0 ) ? 4 : pMUD_array->elemSize, pData );
      break;
    default:
      bcopy( pMUD_array->pData, pData, pMUD_array->num*pMUD_array->elemSize );
      break;
  }

  return( 1 ); 
}

int
MUD_setIndVarData( fd, num, pData )
    int fd;
    int num;
    void* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 
  _free( pMUD_array->pData );
  switch( pMUD_array->elemSize )
  {
    case 0:
      pMUD_array->pData = (caddr_t)zalloc( 4*pMUD_array->num + 32 );
      break;
    default:
      pMUD_array->pData = (caddr_t)zalloc( pMUD_array->num*pMUD_array->elemSize );
      break;
  }

  switch( pMUD_array->type )
  {
    case 1:
      /*
       *  Do packing/byte swapping
       */
      MUD_pack( pMUD_array->num, 
                ( pMUD_array->elemSize == 0 ) ? 4 : pMUD_array->elemSize, pData,
                pMUD_array->elemSize, pMUD_array->pData );
      break;
    default:
      bcopy( pData, pMUD_array->pData, pMUD_array->num*pMUD_array->elemSize );
      break;
  }

  return( 1 ); 
}

int
MUD_getIndVarpTimeData( fd, num, ppData )
    int fd;
    int num;
    UINT32** ppData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 
  *ppData = (UINT32*)pMUD_array->pTime;
  return( 1 ); 
}

int
MUD_setIndVarpTimeData( fd, num, pData )
    int fd;
    int num;
    UINT32* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 
  pMUD_array->pTime = (TIME*)pData;
  return( 1 ); 
}

int
MUD_getIndVarTimeData( fd, num, pData )
    int fd;
    int num;
    UINT32* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 
  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num ); 

  /* 
   *  Already byte swapped
   */
  bcopy( pMUD_array->pTime, pData, 4*pMUD_array->num );

  return( 1 ); 
}

int
MUD_setIndVarTimeData( fd, num, pData )
    int fd;
    int num;
    UINT32* pData;
{
  MUD_SEC_GRP* pMUD_indVarGrp; 
  MUD_SEC_GEN_ARRAY* pMUD_array; 

  _check_fd( fd ); 
  _sea_indvargrp( fd ); 
  _sea_indvardat( fd, num );
  _free( pMUD_array->pTime );

  pMUD_array->pTime = (TIME*)zalloc( 4*pMUD_array->num );
  /* 
   *  Don't byte swap here
   */
  bcopy( pData, pMUD_array->pTime, 4*pMUD_array->num );
  pMUD_array->hasTime = 1;

  return( 1 ); 
}

int 
MUD_getHistSecondsPerBin( int fd, int num, REAL64* pSecondsPerBin )
{
  int i;
  UINT32 fsPerBin;

  if( i = MUD_getHistFsPerBin( fd, num, &fsPerBin ) )
  {
    if( fsPerBin < 16 )
    {
      *pSecondsPerBin = (REAL64)(0.078125e-9 * pow( (double)2.0, (double)(fsPerBin) ));
    }
    else if( fsPerBin < 29 )
    {
      *pSecondsPerBin = (REAL64)(0.048828125e-9 * pow( (double)2.0, (double)(fsPerBin - 16) ));
    }
    else
    {
      *pSecondsPerBin = (REAL64)((double)fsPerBin * 1.0e-15);
    }
  }
  return( i );
}

int 
MUD_setHistSecondsPerBin( int fd, int num, REAL64 secondsPerBin )
{
  if( secondsPerBin < 0.0 || secondsPerBin > 4294967295.0e-15 )
    return( 0 );
  return( MUD_setHistFsPerBin( fd, num, (1.0e15 * (UINT32)secondsPerBin ) ) );
}
