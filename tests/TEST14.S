main(argc, argv)
{
   session = session_open(1, 1);

   while (Input = receive(Session)) {
      if (strncmp(Input, Input, 5) == 0) {
         send(Session, "Test1");
         return;
      }
   }

}

