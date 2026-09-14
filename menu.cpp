#include<iostream>
using namespace std;
void displayMENU(){
    cout<<"=================================================\n";
    cout<<"ADAPTIVE EMERGENCY EVACUATION SYSTEM \n";
    cout<<"1. Display Locations \n";
    cout<<"2. Display Roads\n";
    cout<< "3. Find Evacuation Route \n";
    cout<< "4. Updated Road Condition\n";
    cout<<"5. Exit \n";


    cout<<"\n ENTER YOUR CHOICE :";
}
int main(){
    int choice;
    


    do {
        displayMENU();
        cin>>choice;

    
switch(choice){
    case 1: cout<<"\n Display locations ....\n";
         break;
    case 2: cout<<"\n Displaying  Roads ....\n";
         break;
    case 3: cout<<"\n Finding best evacuation route...\n";
         break;
    case 4: cout<<"\n Updating road condition ...\n";
         break;
    case 5: cout<<"\n  Thank you for using the system !\n";
         break;

    default:cout<<"\n Invalid choice!! Please try again ";

}
    }
while(choice!=5);

return 0;}
