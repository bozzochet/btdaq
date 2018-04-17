{
  char *lname = "libEvent.so";
  printf("Loading %s ... ", lname);
  gSystem->Load("./libEvent.so");
  printf("done\n");
}
