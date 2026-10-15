#include <iostream>
#include <string>
using namespace std ;

int USER_NAME_SIZE , CONTEXT_SIZE ,  QUERY_SIZE , INTERRUPT = false;
string HTTPS= "https" , GITHUBIO = "githubio" , QUERY = "query" ;

string make_github_url(const string& ); 
string extract_user_name(string); //can not be null
string extract_context(string); //can not be null
string extract_query(string); 

int main(){
    string mistyped_address , github_url; 

    cin>> mistyped_address ;
    github_url = make_github_url(mistyped_address);// the code is only robust to odd user names.

    cout<< github_url << endl;

    return 0; 
}


string make_github_url(const string& mistyped_address){
    string url;

    string user_name = extract_user_name(mistyped_address);
    string context = extract_context(mistyped_address);
    string query = extract_query(mistyped_address); 
    if (!INTERRUPT){
        if (query.size() >0){
            url = "https://" + user_name + ".github.io/" + context + "/query=" + query ;
            //eg. :  "https://parsasaeednia.github.io/montecarlo/query=pi"
            return url;
        }
        url = "https://" + user_name + ".github.io/" + context ;
        //eg. :  "https://parsasaeednia.github.io/montecarlo
        return url;
    }
    cerr<<"Invalid format!";
    return "" ;
}

string extract_user_name(string mistyped_address){
    int githubio_loc;

    mistyped_address.erase(0 , HTTPS.size());

    // Find the LAST occurrence of "githubio" (the site address)
    // This prevents confusion if "githubio" appears in the username
    githubio_loc = mistyped_address.rfind(GITHUBIO);
    
    if (githubio_loc != string::npos) {
        // Extract everything before the last "githubio"
        string username = mistyped_address.substr(0, githubio_loc);
        USER_NAME_SIZE = username.size(); // Assuming that we are not alowed to use classes yet, I defined it as a GLOBAL VAR. 
        return username;
    }

    cerr<<"User name can not be null."<<endl;
    INTERRUPT = true;
    return "";
}

string extract_context(string mistyped_address){
    int query_loc; 
    mistyped_address.erase(0 , HTTPS.size()+ USER_NAME_SIZE + GITHUBIO.size());
    query_loc = mistyped_address.rfind(QUERY);
    
    if (query_loc != string::npos) {
        // Extract everything before the last "githubio"
        string context = mistyped_address.substr(0, query_loc);
        CONTEXT_SIZE = context.size(); // Assuming that we are not alowed to use classes yet, I defined it as a GLOBAL VAR. 

        return context;
    }

    cerr<<"Context can not be null."<<endl;
    INTERRUPT = true;
    return "";
}

string extract_query(string mistyped_address){
    string query =mistyped_address.erase(0 , HTTPS.size()+ USER_NAME_SIZE + GITHUBIO.size()+ CONTEXT_SIZE + QUERY.size());
    
    if (query.size() > 0) {
        return query;
    }

    return "";
}