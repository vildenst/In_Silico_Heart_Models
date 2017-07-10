#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if ( strcmp(argv[1],"help") == 0)
    {
      printf("\nUsage: %s vtk_file model_base_name\n", argv[0] );
      printf("Converts vtk file to carp format (outputs .pts, .elem, .lon, and .tris)\n");
      printf(" * If Part_Id=0 in vtk file, .lon outputs '0 0 0'\n");
      printf(" * If Part_Id>0 in vtk file, .lon outputs '1 0 0'\n");
      printf(" * .tris will be empty unless triangular elements are detected in .vtk file\n");
      printf(" * Use Meshalyzer 'Compute Surfaces' to generate surface files if needed\n");
      printf("Written by Hermenegild Arevalo, August 12, 2014\n\n");
      exit(0);
    }

  if ( argc != 3 ) 
    {
      printf( "\nUsage: %s vtk_file model_base_name\n", argv[0] );
      printf( "type '%s help' for help\n\n", argv[0] );
      exit(0);
    }

  else
    {  
      FILE *input, *output, *input2, *output2;
      char read[200],x[100],y[100],z[100],a[200],b[200],c[200],d[200],e[200],f[200],g[200],h[200];
      int i,j,k,type,tiss,count,temp,numtissid;
      int m,n,o,p;
      
      input=fopen(argv[1],"r");
      sprintf(read,"%s.pts",argv[2]);
      output=fopen(read,"w");
      
      
      while(strcmp(read,"$Nodes"))
	{
	  fscanf(input,"%s",read);
	}
      
      fscanf(input,"%d",&i);
      fprintf(output,"%d\n",i);

      for(j=0;j<i;j++)
	{
	  fscanf(input,"%s %s %s %s",a,x,y,z);
	  fprintf(output,"%s %s %s\n",x,y,z);
	}
      
      fclose(output);

      sprintf(read,"%s.elem",argv[2]);
      output=fopen(read,"w");
      sprintf(read,"%s.tris",argv[2]);
      output2=fopen(read,"w");

      while(strcmp(read,"$Elements"))
      	{
      	  fscanf(input,"%s",read);
	  //printf("%s\n",read);
      	}
      
      fscanf(input,"%d",&i);
      fprintf(output,"%d\n",i);
      //fscanf(input,"%s",read);
      
      count=0;
      for(j=0;j<i;j++)
      	{
      	  fscanf(input,"%d %d",&temp,&type);

	  //printf("%d %d \n", temp, type);

      	  if(type==4)
      	    {
      	      fscanf(input,"%d",&numtissid);
      	      for(k=0;k<numtissid;k++)
      		{
      		 fscanf(input,"%d",&temp);
      		}
      	      fscanf(input,"%d %d %d %d",&m,&n,&o,&p);
      	      fprintf(output,"Tt %d %d %d %d 1\n",m-1,n-1,o-1,p-1);
      	      count++;
      	    }
	  if(type==2)
      	    {
      	      fscanf(input,"%d",&numtissid);
      	      for(k=0;k<numtissid;k++)
      		{
      		 fscanf(input,"%d",&temp);
      		}
      	      fscanf(input,"%d %d %d",&m,&n,&o);
      	      fprintf(output2,"%d %d %d 1\n",m-1,n-1,o-1);
      	    }
	  if(type==1)
	    {
	      fscanf(input,"%d",&numtissid);
      	      for(k=0;k<numtissid;k++)
      		{
		  fscanf(input,"%d",&temp);
      		}
      	      fscanf(input,"%d %d",&m,&n);
	    }
      	}
      
      fclose(output);
      fclose(output2);
  
      /* sprintf(read,".%s.elem_tmp",argv[2]); */
      /* input2=fopen(read,"r"); */
      /* sprintf(read,"%s.elem",argv[2]); */
      /* output=fopen(read,"w"); */
      /* sprintf(read,"%s.lon",argv[2]); */
      /* output2=fopen(read,"w"); */

      /* fscanf(input2,"%d",&i); */
      /* fprintf(output,"%d\n",count); */
      
      /* for(j=0;j<count;j++) */
      /* 	{ */
      /* 	  fscanf(input,"%d",&tiss); */

      /* 	  fscanf(input2,"%d",&type); */
      /* 	  switch(type){ */
      /* 	    //case 3: */
      /* 	    //fscanf(input2,"%s %s %s",a,b,c); */
      /* 	    //fprintf(output,"Tr %s %s %s %d\n",a,b,c,tiss); */
      /* 	    //break; */
      /* 	  case 4: */
      /* 	    fscanf(input2,"%s %s %s %s",a,b,c,d); */
      /* 	    fprintf(output,"Tt %s %s %s %s %d\n",a,b,c,d,tiss); */
      /* 	    break; */
      /* 	  case 5: */
      /* 	    fscanf(input2,"%s %s %s %s %s",a,b,c,d,e); */
      /* 	    fprintf(output,"Py %s %s %s %s %s %d\n",a,b,c,d,e,tiss); */
      /* 	    break; */
      /* 	  case 6: */
      /* 	    fscanf(input2,"%s %s %s %s %s %s",a,b,c,d,e,f); */
      /* 	    fprintf(output,"Pr %s %s %s %s %s %s %d\n",a,b,c,d,e,f,tiss); */
      /* 	    break; */
      /* 	  case 8: */
      /* 	    fscanf(input2,"%s %s %s %s %s %s %s %s",a,b,c,d,e,f,g,h); */
      /* 	    fprintf(output,"Hx %s %s %s %s %s %s %s %s %d\n",a,b,c,d,e,f,g,h,tiss); */
      /* 	    break; */
      /* 	  } */
      /* 	  if(tiss==0) */
      /* 	    fprintf(output2,"0 0 0\n"); */
      /* 	  else */
      /* 	    fprintf(output2,"1 0 0\n"); */
      /* 	} */
      
      /* fclose(input); */
      /* fclose(input2); */
      /* fclose(output); */
      /* fclose(output2); */
      /* /\* sprintf(read,".%s.elem_tmp",argv[2]); *\/ */
      /* /\* remove(read); *\/ */
    }
}
