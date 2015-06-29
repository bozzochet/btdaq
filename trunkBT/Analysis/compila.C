{
  gSystem->Load("./libEvent.so");

  //  printf("%s\n", gSystem->GetMakeSharedLib());
  gSystem->SetMakeSharedLib("cd $BuildDir ; clang++ -c $Opt -m64 -pipe -W -fsigned-char -fno-common -stdlib=libc++ -pthread $IncludePath $SourceFiles ; clang++ $ObjectFiles  -dynamiclib -single_module -Wl,-dead_strip_dylibs -O2 -m64 -mmacosx-version-min=10.9 -stdlib=libc++ $LinkedLibs -o $SharedLib");
  //  printf("%s\n", gSystem->GetMakeSharedLib());
  gSystem->AddIncludePath("-I../Decode");

  gSystem->CompileMacro("macro.C");
}
