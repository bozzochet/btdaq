#ifndef _cluster_included_
#define _cluster_included_



typedef struct header {
  int NRun;
  int NEvt;
  int S_clus;
  int K_clus;
} header;

typedef struct channel {
  int    Slot;
  double ADC;
  double CN;
  int    Status;
  struct channel *next; // Next channel found =NULL if none
}channel;

typedef struct channel_write {
  int    Slot;
  double ADC;
  double CN;
  int    Status;
}channel_write;


typedef struct cluster {
  int first;                // First strip of the cluster
  int length;               // Length of the cluster last=first+length 
  float cog;                // Center of gravity
  int maxloc;               // Strip with highest signal
  float maxval;             // Highest signal
  float integral;           // Total charge collected    
  float sovern1;             // S/N (integral/sqrt(sigma**2))
  float sovern2;             // S/N (integral/sigma mean)
  float sovern3;             // S/N (sqrt(sum((signal/sigma)**2))
  struct channel *channel;  // Pointer to channel list
  struct cluster *next;     // Next cluster found =NULL if none
}cluster;

typedef struct cluster_write {
  int first;                // First strip of the cluster
  int length;               // Length of the cluster last=first+length 
  float cog;                // Center of gravity
  int maxloc;               // Strip with highest signal
  float maxval;             // Highest signal
  float integral;           // Total charge collected    
  float sovern1;             // S/N (integral/sqrt(sigma**2))
  float sovern2;             // S/N (integral/sigma mean)
  float sovern3;             // S/N (sqrt(sum((signal/sigma)**2))
}cluster_write;

#endif /* _cluster_included_ */





