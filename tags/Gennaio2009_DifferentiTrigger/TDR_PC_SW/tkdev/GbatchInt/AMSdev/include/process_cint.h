#ifdef __CINT__
      void Begin(TTree* tree){
            AMSEventR::Begin(tree);
            UBegin();
      };

      Bool_t ProcessCut(Long64_t entry){
            return ProcessCut((int)entry);
      };

      void ProcessFill(Long64_t entry){
            ProcessFill((int)entry);
      };

      Bool_t ProcessCut(int entry){
            return ReadHeader(entry);
      };

      void ProcessFill(int entry){
            AMSEventR::ProcessFill(entry);
            if (!UProcessCut()) return;
            UProcessFill();
      };

      void Terminate(){
            UTerminate();
            AMSEventR::Terminate();
      };
#endif
