#include<iostream>
using namespace std;
void displayMENU(){
    cout<<"ADAPTIVE EMERGENCY EVACUATION SYSTEM "<<endl;
    cout<<"1. Display Locations "<<endl;
    cout<<"2. Display Roads"<<endl;
    cout<< "3. Find Evacuation Route"<<endl;
    cout<< "4. Updated Road Condition"<<endl;
    cout<<"5. Exit"<<endl;
    cout<<"ENTER YOUR CHOICE :"<<endl;
}
int main(){
    int choice;
    do {
        displayMENU();
        cin>>choice;
switch(choice){
    case 1: cout<<" Display locations "<<endl;
         break;
    case 2: cout<< "Displaying  Roads"<<endl;
         break;
    case 3: cout<<" Finding best evacuation route"<<endl;
         break;
    case 4: cout<<" Updating road condition"<<endl;
         break;
    case 5: cout<<"Thank you for using the system !"<<endl;
         break;
    default:cout<<"Invalid choice!! Please try again "<<endl;
}
    }
while(choice!=5);

return 0;}
