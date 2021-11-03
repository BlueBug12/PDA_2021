
int main(int argc, char ** argv){
   std::string out_name;
      if(argc == 2){
         out_name = "output.txt";
      }else if(argc ==3){
         out_name = argv[2];
      }else{
         cerr << "Error: wrong parameters" << endl;
         exit(1);
      }

      float balance_ratio = 0.5;
      FM fm(argv[1], balance_ratio);

   return 0;
}