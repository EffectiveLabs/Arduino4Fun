      #include <Servo.h>
     
    Servo monServo;
    int positionServo=0;
    int etatbouton=0;
    
    void setup()
    {
      pinMode(2,INPUT);
      monServo.attach(9);
      monServo.write(positionServo);
    }
    void loop()
   {
     etatbouton=digitalRead(2);
     if(etatbouton!=0){
       ouvrirRobinetCroquettes(2750);
       fermerRobinetCroquettes();
     }
   } 
   
   void ouvrirRobinetCroquettes(int tempsAttente){
       positionServo = 180;
       monServo.write(positionServo);
       delay(tempsAttente);
   }
   
   
   void fermerRobinetCroquettes(){
      positionServo = -180;
       monServo.write(positionServo);
       int tempsAttente = 250;
       delay(tempsAttente);
       
   }
