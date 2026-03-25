/*
DESCRIEREA PROBLEMEI:
Programul simuleaza un sistem de gestiune a rezervarilor
pentru un hotel cu pana la 50 de camere de tipuri diferite.
Rezervarile se fac pe "ziua anului" (numere intregi 1-365),
unde ziua 1 = 1 ianuarie si ziua 365 = 31 decembrie.
sirurile de caractere sunt gestionate cu char* alocat dinamic (new[]/delete[]).
DATE DE INTRARE:
- Camera : numar (identificator unic), tip (SINGLE /DOUBLE / SUITE / PENTHOUSE), pret pe noapte (RON), capacitate maxima de persoane
- Client : CNP (13 cifre), nume, prenume, email, telefon
- Cerere de rezervare: tipul camerei dorite, ziua de check-in (1-365), ziua de check-out (1-365), numarul de persoane
OPERATII POSIBILE:
1. adaugaCamera() - adauga o camera in vectorul fix al hotelului (maxim 50 camere)
2. esteValabila() [FUNCTIE COMPLEXA] - verifica daca o camera este libera intr-un interval de zile [checkin, checkout); parcurge vectorul de rezervari si detecteaza suprapunerile prin
   conditia: NU(checkin>=ziCheckout SAU checkout<=ziCheckin). Camera nu are niciun camp boolean "ocupat" - devine indisponibila strict prin existenta unei rezervari in vector
3. cereRezervare() [FUNCTIE COMPLEXA] - primeste cererea unui client (tip camera, interval, nr persoane); parcurge camerele, cauta prima de tipul dorit libera in perioada ceruta (apeland esteValabila),
   calculeaza pretul total = nopti * pret_noapte si inregistreaza rezervarea in vectorul dinamic, blocand astfel camera pentru acea perioada
CLASE:
- Camera: camera de hotel (tip char* dinamic, pret, capacitate); contine getteri, setteri, citeste(), operator<<
- Client: date personale cu campuri char* dinamice; contine getteri, setteri, citeste(), getNumeComplet(), operator<<
- Rezervare : leaga un Client de o Camera pe un interval [zi_checkin, zi_checkout); contine getteri, setteri, operator<<
- Hotel: vector fix Camera[50] + vector dinamic Rezervare* alocat cu new[]/delete[]; contine functiile complexe de business si operator<<
============================================================
 */


#include <iostream>
#include <cstring>

class Camera{
int numar;
double pret_noapte;
int capacitate_max;
char* tip; //folosim pointer pentru alocare dinamică a șirului de caractere

/// functie privata de copiere
void copiazaTip(const char* t) {
        tip=new char[strlen(t)+1];  //alocăm exact cât spațiu e nevoie (+1 pentru nul)
        strcpy(tip, t);  //copiem conținutul efectiv în zona nouă de memorie
    }

public:

    //constructor implicit
    Camera(): numar(0), pret_noapte(0.0), capacitate_max(1)
    { copiazaTip("SINGLE"); //setăm o valoare default ca să nu avem tip=null
    }

    ///constructor parametrizat
    Camera(const int numar, const double pret_noapte, const int capacitate_max, const char* tip){
        this->numar=numar;
        this->pret_noapte=pret_noapte;
        copiazaTip(tip); //apelăm funcția de alocare pentru a evita duplicarea codului
        this->capacitate_max=capacitate_max;
        //std::cout << "Constr de inițializare Camera\n";
    }

    //constructor de copiere
    Camera(const Camera& alta){
        this->numar=alta.numar;
        this->pret_noapte=alta.pret_noapte;
        copiazaTip(alta.tip); //facem deep copy, nu copiem doar adresa pointerului
        this->capacitate_max=alta.capacitate_max;
        //std::cout << "Constr de copiere Camera\n";
    }

    //operator = pentru atribuire
    Camera& operator=(const Camera& alta) {
        if(this != &alta) {
            delete[] tip; //eliberam memoria veche înainte să alocăm ceva nou
            numar=alta.numar;
            pret_noapte=alta.pret_noapte;
            copiazaTip(alta.tip); //alocăm spațiu nou pentru datele copiate
            capacitate_max=alta.capacitate_max;
        }
        return *this;
    }


    //destructor

    ~Camera() {
        delete[] tip;
        //std::cout << "Destructor Camera\n";
    }

    ///getteri- folosim const pentru că aceste funcții doar citesc date, nu le modifică
    int getNumar()const { return numar;}
    const char* getTip() const { return tip;}
    double getPretPeNoapte() const { return pret_noapte;}
    int getCapacitateMaxima() const { return capacitate_max;}


    ///setteri
    void setNumar(int n) { numar=n;}
    void setPretPeNoapte(double p) { pret_noapte=p;}
    void setCapacitateMaxima(int c) { capacitate_max=c;}
    void setTip(const char* t) {
        delete[] tip; //eliberam ce era inainte
        copiazaTip(t); //alocam noul tip
    }

    void citeste() {
        char buf[50]; //buffer temporar pt citire de la tastatura
        std::cout << "  Numar camera: "; std::cin >> numar;
        std::cout << "  Tip (SINGLE/DOUBLE/SUITE/PENTHOUSE): ";
        std::cin >> buf;
        setTip(buf); //folosim setterul care se ocupă deja de managementul memoriei
        std::cout << "  Pret/noapte: "; std::cin >> pret_noapte;
        std::cout << "  Capacitate max: "; std::cin >> capacitate_max;
    }

    ///operator de afisare, afiseaza toate detaliile despre camera: numar,tip,pret si capacitate maxima
    friend std::ostream& operator<<(std::ostream& os, const Camera& c) {
        os<<"Camera #"<<c.numar<<" | Tip: "<<c.tip<< " | Pret/noapte: "<<c.pret_noapte<<" RON"<<" | Capacitate: "<<c.capacitate_max<<" pers.";
        return os;
    }

};


class Client{
char* cnp;
char* nume;
char* prenume;
char* email;
char* telefon;

///Functie privata care aloca un nou sir si copiaza continutul
static char* copiazaSir(const char* src) {
        char* dest=new char[strlen(src)+1];
        strcpy(dest,src);
        return dest;
    }

/// Functie privata care elibereaza toti pointerii
 void elibereaza() {
        delete[] cnp;
        delete[] nume;
        delete[] prenume;
        delete[] email;
        delete[] telefon;
    }

    /// Functie privata care copiaza toate campurile dintr-un alt client
void copiazaDin(const Client& altul) {
        cnp=copiazaSir(altul.cnp);
        nume=copiazaSir(altul.nume);
        prenume=copiazaSir(altul.prenume);
        email=copiazaSir(altul.email);
        telefon=copiazaSir(altul.telefon);
    }
public:

    ///constr implicit
    Client() {
        cnp=copiazaSir("");
        nume=copiazaSir("");
        prenume=copiazaSir("");
        email=copiazaSir("");
        telefon=copiazaSir("");
    }

    //constructor de initializare

    Client(const char* cnp, const char* nume, const char* prenume, const char* email, const char* telefon) {
        this->cnp=copiazaSir(cnp);
        this->nume=copiazaSir(nume);
        this->prenume=copiazaSir(prenume);
        this->email=copiazaSir(email);
        this->telefon=copiazaSir(telefon);
        //std::cout << "Constr de initializare Client\n";
    }

    /// constructor de copiere
    Client(const Client& altul) {
        copiazaDin(altul);
        //std::cout << "Constr de copiere Client\n";
    }

    ///operator = pentru atribuire
    Client& operator=(const Client& altul) {
        if (this!=&altul) {
            elibereaza(); // ștergem ce aveam înainte să punem noile date
            copiazaDin(altul);
        }
        return *this;
    }

    ///destructor
    ~Client() {
        elibereaza();
        //std::cout << "Destructor Client\n";
    }

    ///getteri

    const char* getCnp() const { return cnp;}
    const char* getNume() const { return nume; }
    const char* getPrenume() const { return prenume; }
    const char* getEmail() const { return email; }
    const char* getTelefon() const { return telefon; }


    ///Setteri
    void setCnp(const char* s) { delete[] cnp; cnp=copiazaSir(s);}
    void setNume(const char* s) { delete[] nume; nume=copiazaSir(s);}
    void setPrenume(const char* s){ delete[] prenume; prenume=copiazaSir(s);}
    void setEmail(const char* s) { delete[] email;email=copiazaSir(s);}
    void setTelefon(const char* s) { delete[] telefon; telefon=copiazaSir(s);}

    //Citeste datele unui client de la tastatura
    void citeste() {
        char buf[100];
        std::cout << "  CNP : "; std::cin >> buf; setCnp(buf);
        std::cout << "  Nume: "; std::cin >> buf; setNume(buf);
        std::cout << "  Prenume: "; std::cin >> buf; setPrenume(buf);
        std::cout << "  Email: "; std::cin >> buf; setEmail(buf);
        std::cout << "  Telefon: "; std::cin >> buf; setTelefon(buf);
    }

    ///opeartor afisare, afiseaza toate datele despre un cleint

    friend std::ostream& operator<<(std::ostream& os, const Client& cl) {
        os<< "Client [CNP: "<<cl.cnp<<"] "<<cl.prenume<< " " <<cl.nume<< " | Email: "<<cl.email<<" | Tel: "<<cl.telefon;
        return os;
    }

};


class Rezervare{
int id_rezervare;
int zi_checkin;
int zi_checkout;
int nrpers;
bool activa;
Client client_titular;
Camera camera_aleasa;

public:

    ///constructor implicit
    Rezervare(): id_rezervare(0), zi_checkin(1), zi_checkout(2), nrpers(1), activa(false) {}



    ///constr de initializare
    Rezervare(const int id_rezervare, const int zi_checkin, const int zi_checkout, const int nrpers, const Client& c, const Camera& cam){
        this->id_rezervare=id_rezervare;
        this->zi_checkin=zi_checkin;
        this->zi_checkout=zi_checkout;
        this->nrpers=nrpers;
        this->activa= true; //o rezervare nou creată e activă implicit
        this->client_titular=c; //se apelează operator= din clasa Client
        this->camera_aleasa=cam;
        //std::cout << "Constr de inițializare Rezervare\n";
    }
    //constr de copiere
    Rezervare(const Rezervare& alta){
        this->id_rezervare=alta.id_rezervare;
        this->zi_checkin=alta.zi_checkin;
        this->zi_checkout=alta.zi_checkout;
        this->nrpers=alta.nrpers;
        this->activa=alta.activa;
        this->client_titular=alta.client_titular;
        this->camera_aleasa=alta.camera_aleasa;
        //std::cout << "Constr de copiere Rezervare\n";
    }


    //operator de atribuire =
    Rezervare& operator=(const Rezervare& alta) {
        if (this!= &alta) {
            id_rezervare=alta.id_rezervare;
            client_titular=alta.client_titular;
            camera_aleasa=alta.camera_aleasa;
            zi_checkin=alta.zi_checkin;
            zi_checkout=alta.zi_checkout;
            nrpers=alta.nrpers;
            activa=alta.activa;
        }
        return *this;
    }


    //destructor
    ~Rezervare() {
        //std::cout << "Destructor Rezervare\n";
    }


    ///getteri
    int getId() const { return id_rezervare; }
    const Client& getClient() const { return client_titular;}
    const Camera& getCamera() const { return camera_aleasa;}
    int getZiCheckIn() const { return zi_checkin;}
    int getZiCheckOut() const { return zi_checkout;}
    int getNopti() const { return zi_checkout - zi_checkin;} //calculam oe loc durata sederii
    bool esteActiva() const { return activa;}


    ///setteri
    void setZiCheckIn(int z) { zi_checkin=z;}
    void setZiCheckOut(int z) { zi_checkout=z;}
    void setNrPers(int n) { nrpers=n;}


    ///operator de afisare, afiseaza toate datele unei rezervari, daca este activa sau nu, si perioada in care este activa
    friend std::ostream& operator<<(std::ostream& os, const Rezervare& r) {
    os<< "Rezervare #" << r.id_rezervare<< (r.activa ? " [ACTIVA]" : " [ANULATA]") << "\n"
       << "  Client : " << r.client_titular.getPrenume() << " " << r.client_titular.getNume()
       << " (CNP: " << r.client_titular.getCnp() << ")\n"
       << "  Camera: " << r.camera_aleasa << "\n"
       << "  Check-in : ziua " << r.zi_checkin << "\n"
       << "  Check-out: ziua " << r.zi_checkout << " (" << r.getNopti() << " nopti)\n"
       << "  Persoane: " << r.nrpers;
    return os;
}
};


class Hotel{
static const int max_camere = 50; //limită hardcodata pentru vectorul static
Camera camere[max_camere]; //vector static de obiecte Camera
Rezervare *rezervari; //vector dinamic de rezervări (crește la nevoie)
int camere_adaugate;
int nr_rez;
int capacitate_rez;
int next_id;

 ///functie privata care cauta indexul camerei cu numarul dat, sau afiseaza -1 daca nu exista

    int gasesteIndexCamera(int nrCamera) const {
        for (int i=0; i<camere_adaugate; ++i)
            if (camere[i].getNumar()==nrCamera)
                return i;
        return -1;
    }
///functie privata care dubleaza capacitatea vectorului dinamic de rezervari
    void extindeRezervari() {
        // Evităm situația în care capacitatea ar fi 0 (ar rezulta în alocări de dimensiune 0).
        if (capacitate_rez < 1) capacitate_rez = 1;
        capacitate_rez*=2; //dublăm spațiul ca să nu alocăm prea des
        Rezervare* nou=new Rezervare[capacitate_rez];
        for (int i=0;i<nr_rez; ++i)
            nou[i]=rezervari[i];  //copiem rezervările vechi în noua locație
        delete[] rezervari;  //eliberăm memoria veche
        rezervari=nou; //mutăm pointerul la noul vector
    }

public:
    ///constructor implicit
    Hotel(int capRez=20) {
        this->camere_adaugate=0;
        this->capacitate_rez=capRez;
        this->nr_rez=0;
        this->next_id=1;
        this->rezervari=new Rezervare[capacitate_rez];

        //std::cout << "Hotel creat cu memorie dinamică.\n";
    }

     ///constructor de copiere
    Hotel( const Hotel& altul){
    this->camere_adaugate=altul.camere_adaugate;
    this->nr_rez=altul.nr_rez;
    this->next_id=altul.next_id;
    this->capacitate_rez=altul.capacitate_rez;
    this->rezervari=new Rezervare[capacitate_rez];
    for(int i=0;i<camere_adaugate;++i) {
        this->camere[i]=altul.camere[i];
            }
    for(int i=0;i<this->nr_rez;++i){
        this->rezervari[i]=altul.rezervari[i];
    }


    }

    /// operator de atribuire =
    Hotel& operator=(const Hotel& alta) {
        if (this!= &alta) {
            delete[] this->rezervari;
            camere_adaugate=alta.camere_adaugate;
            nr_rez=alta.nr_rez;
            this->next_id=alta.next_id;
            capacitate_rez=alta.capacitate_rez;
            for(int i=0; i<camere_adaugate; ++i) {
                this->camere[i]=alta.camere[i];
            }
            this->rezervari = new Rezervare[this->capacitate_rez];
            for(int i=0; i<this->nr_rez; ++i) {
                this->rezervari[i]=alta.rezervari[i];
            }

        }
        return *this;
    }

    //destructor
    ~Hotel() {
        delete[] rezervari;
    }

    void adaugaCamera(const Camera& c) { // Inserează camera `c` în array-ul static.
        if (camere_adaugate>=max_camere) {
            std::cout << "Hotelul este plin (max 50 camere)!\n";
            return; //Ieșim fără a adăuga camera.
        }
        camere[camere_adaugate++]=c; // Adăugăm camera și incrementăm contorul
    }

    ///functie complexa care verifica daca o camera ceruta este valabila in perioada dorita, functia va returna false daca exista deja o rezervare pe acea camera
    ///care se suprapune cu perioada ceruta de client
    bool esteValabila(int nrCamera, int checkin, int checkout) const {
        if (checkin>=checkout)
            return false;
        for (int i=0; i<nr_rez;++i) {
            if (rezervari[i].getCamera().getNumar()!=nrCamera)
            continue;
            if (!rezervari[i].esteActiva())
            continue;

            int ziCheckin=rezervari[i].getZiCheckIn();
            int ziCheckout=rezervari[i].getZiCheckOut();

            if (!(checkin>=ziCheckout || checkout<=ziCheckin))
                return false;
        }
        return true;
    }



    //  Primeste cererea unui client (tip camera dorit, interval de zile, numar persoane). Parcurge camerele hotelului si cauta prima camera de tipul dorit care:
    //   - are capacitate suficienta pentru nrpers
    //   - este libera in [checkin, checkout) (apeland esteValabila)
    //  Daca o gaseste: calculeaza pretul total = nopti * pret_noapte
    //  si adauga rezervarea in vectorul dinamic, blocand astfel
    //  camera pentru acea perioada la urmatoarea cautare.
    //  Returneaza id-ul rezervarii create, sau -1 la esec.

    int cereRezervare(const Client& client, const char* tipDorit, int checkin, int checkout, int nrpers) {
         if (tipDorit == nullptr || tipDorit[0] == '\0') {
            std::cout << "Tip camera invalid!\n";
            return -1;
        }
        if (nrpers < 1) {
            std::cout << "Nr. persoane invalid!\n";
            return -1;
        }
        if (checkin <1 || checkout>365 || checkin>=checkout) {
            std::cout << "Interval de zile invalid!\n";
            return -1;
        }

        for (int i=0; i<camere_adaugate;++i) {
            if (std::strcmp(camere[i].getTip(), tipDorit)!=0)
                continue;
            if (nrpers>camere[i].getCapacitateMaxima())
                continue;

            /// verificam daca camera este libera in perioada ceruta?
            if (!esteValabila(camere[i].getNumar(), checkin, checkout))
                continue;

            ///  Calculam pretul total
            int nopti=checkout-checkin;
            double pret=nopti*camere[i].getPretPeNoapte();

            /// Adaugam rezervarea in vectorul dinamic
            if (nr_rez==capacitate_rez)
                extindeRezervari();
            rezervari[nr_rez++]=Rezervare(next_id, checkin, checkout,nrpers, client, camere[i]);

        std::cout << "  [CONFIRMAT] Rezervare #" << next_id << "\n"
          << "  Client  : " << client.getPrenume() << " " << client.getNume() << "\n"
          << "  Camera  : #" << camere[i].getNumar() << " (" << camere[i].getTip() << ")\n"
          << "  Perioada: ziua " << checkin << " -> ziua " << checkout << " (" << nopti << " nopti)\n"
          << "  TOTAL   : " << pret << " RON\n";

            return next_id++;
        }

        std::cout << "Nicio camera de tip '" << tipDorit<< "' disponibila in perioada ziua "<< checkin << " - ziua " << checkout << ".\n";
        return -1;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hotel& h) {
        int active=0;
        for (int i=0; i<h.nr_rez; ++i)
            if (h.rezervari[i].esteActiva())
                ++active;
        os << "---HOTEL---\n"<< "  Camere inregistrate : " << h.camere_adaugate << "\n"<< "  Rezervari active : " << active << " / " << h.nr_rez << "\n";
        return os;
    }




};


/*
int main()
{
    std::cout << "-------------------------------------------------------\n";
    std::cout << "   SISTEM DE REZERVARI HOTEL\n";
    std::cout << "-------------------------------------------------------\n";

    Hotel hotel(20);

    //Citire camere
    int nrCamere;
    std::cout << "\nCate camere doriti sa adaugati? ";
    std::cin >> nrCamere;

    for (int i = 0; i < nrCamere; ++i) {
        std::cout << "\n--- Camera " << (i + 1) << " ---\n";
        Camera c;
        c.citeste();
        hotel.adaugaCamera(c);
    }

    std::cout << "\n" << hotel << "\n";

    //Citire clienti si rezervari
    int nrRezervari;
    std::cout << "Cate rezervari doriti sa faceti? ";
    std::cin >> nrRezervari;

    for (int i = 0; i < nrRezervari; ++i) {
        std::cout << "\n-----------------------------------------------------\n";
        std::cout << "  Rezervare " << (i + 1) << "\n";
        std::cout << "-------------------------------------------------------\n";

        //Citim datele clientului
        std::cout << "\nDate client:\n";
        Client client;
        client.citeste();

        //Citim preferintele de rezervare
        char tipDorit[50];
        int checkin, checkout, nrpers;

        std::cout << "\nTip camera dorit (SINGLE/DOUBLE/SUITE/PENTHOUSE): ";
        std::cin >> tipDorit;
        std::cout << "Zi check-in  (1-365): ";
        std::cin >> checkin;
        std::cout << "Zi check-out (1-365): ";
        std::cin >> checkout;
        std::cout << "Nr. persoane : ";
        std::cin >> nrpers;

        int id = hotel.cereRezervare(client, tipDorit, checkin, checkout, nrpers);
        if (id != -1)
            std::cout << "[OK] Rezervare inregistrata cu ID: " << id << "\n";
        else
            std::cout << "[ESEC] Rezervarea nu a putut fi efectuata.\n";
    }

    //Starea finala
    std::cout << "\n" << hotel << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "   Multumim! Zi frumoasa!.\n";
    std::cout << "-------------------------------------------------------\n";

    return 0;
}

*/

int main() {
    std::cout << "-------------------------------------------------------\n";
    std::cout << "   SISTEM DE REZERVARI HOTEL\n";
    std::cout << "-------------------------------------------------------\n";

    Hotel hotel(20);

    // ------ Camere initializate direct cu constructorul ------
    hotel.adaugaCamera(Camera(101, 250.0,  1, "SINGLE"));
    hotel.adaugaCamera(Camera(102, 250.0,  1, "SINGLE"));
    hotel.adaugaCamera(Camera(103, 250.0,  1, "SINGLE"));
    hotel.adaugaCamera(Camera(201, 420.0,  2, "DOUBLE"));
    hotel.adaugaCamera(Camera(202, 420.0,  2, "DOUBLE"));
    hotel.adaugaCamera(Camera(301, 800.0,  3, "SUITE"));
    hotel.adaugaCamera(Camera(401, 1500.0, 4, "PENTHOUSE"));

    std::cout << "\n" << hotel << "\n";

    // ------ Clienti initializati direct cu constructorul ------
    Client ion    ("1900101400015", "Popescu",  "Ion", "ion.p@mail.ro",   "0721000001");
    Client maria  ("2850615440022", "Ionescu",  "Maria", "maria.i@mail.ro", "0722000002");
    Client andrei ("1780320300033", "Gheorghe", "Andrei", "a.g@firma.ro",    "0733000003");
    Client elena  ("2920804210044", "Stan",     "Elena", "e.stan@mail.ro",  "0744000004");

    std::cout << "-------------------------------------------------------\n";
    std::cout << "  TEST 1: Rezervari normale (ar trebui sa reuseasca)\n";
    std::cout << "-------------------------------------------------------\n";

    // Ion rezerva SINGLE zilele 150-153 (3 nopti = 750 RON)
    hotel.cereRezervare(ion, "SINGLE", 150, 153, 1);

    // Maria rezerva DOUBLE zilele 160-167 (7 nopti = 2940 RON)
    hotel.cereRezervare(maria,  "DOUBLE", 160, 167, 2);

    // Andrei rezerva SUITE zilele 200-214 (14 nopti = 11200 RON)
    hotel.cereRezervare(andrei, "SUITE", 200, 214, 2);

    // Elena rezerva PENTHOUSE zilele 250-255 (5 nopti = 7500 RON)
    hotel.cereRezervare(elena, "PENTHOUSE", 250, 255, 3);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 2: Rezervare suprapusa pe aceeasi camera\n";
    std::cout << "  Camera 101 e ocupata 150-153; Ion cere 151-155\n";
    std::cout << "  -> 101 e ocupata, dar 102 e libera => se aloca 102\n";
    std::cout << "-------------------------------------------------------\n";

    hotel.cereRezervare(ion, "SINGLE", 151, 155, 1);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 3: Tip de camera indisponibil in perioada ceruta\n";
    std::cout << "  Singura SUITE e ocupata 200-214; Maria cere 205-210\n";
    std::cout << "  -> nicio SUITE disponibila => ESEC\n";
    std::cout << "-------------------------------------------------------\n";

    hotel.cereRezervare(maria, "SUITE", 205, 210, 2);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 4: Interval de zile invalid\n";
    std::cout << "  checkin=100, checkout=50 => checkin >= checkout\n";
    std::cout << "-------------------------------------------------------\n";

    hotel.cereRezervare(andrei, "DOUBLE", 100, 50, 1);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 5: Prea multe persoane pentru tipul ales\n";
    std::cout << "  SINGLE are capacitate 1; Elena cere 3 persoane\n";
    std::cout << "  => nicio SINGLE nu accepta 3 pers => ESEC\n";
    std::cout << "-------------------------------------------------------\n";

    hotel.cereRezervare(elena, "SINGLE", 10, 15, 3);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 6: Rezervare imediat dupa un interval ocupat\n";
    std::cout << "  Camera 101 e ocupata 150-153; Ion cere 153-158\n";
    std::cout << "  -> checkout=153 == checkin_nou=153 => fara suprapunere\n";
    std::cout << "  => rezervare reusita pe 101\n";
    std::cout << "-------------------------------------------------------\n";

    hotel.cereRezervare(ion, "SINGLE", 153, 158, 1);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "  TEST 7: Validari input (nr persoane invalid + tip camera invalid)\n";
    std::cout << "  - Elena cere 0 persoane => ESEC (nr persoane invalid)\n";
    std::cout << "  - Ion cere VILLA => ESEC (tip camera invalid/inexistent)\n";
    std::cout << "-------------------------------------------------------\n";

     // Elena cere un numar invalid de persoane (0)
    hotel.cereRezervare(elena, "SINGLE", 300, 302, 0);

     // Ion cere un tip de camera care nu exista in lista adaugata
    hotel.cereRezervare(ion, "VILLA", 310, 312, 1);


    // Starea finala
    std::cout << "\n" << hotel << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "   Multumim! Zi frumoasa!\n";
    std::cout << "-------------------------------------------------------\n";

    return 0;
}

