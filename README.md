# Sistem de Gestiune a Rezervărilor Hotel

Program C++ care simulează un sistem de rezervări pentru un hotel cu până la 50 de camere, implementat folosind programare orientată pe obiecte.

---

## Descrierea Problemei

Rezervările se fac pe **ziua anului** (numere întregi 1–365), unde ziua 1 = 1 ianuarie și ziua 365 = 31 decembrie. Intervalul unei rezervări este semi-deschis **[checkin, checkout)** — checkout-ul unei rezervări coincide cu checkin-ul următoarei fără suprapunere. Șirurile de caractere sunt gestionate cu `char*` alocat dinamic (`new[]`/`delete[]`).

### Date de intrare

| Entitate | Câmpuri |
|---|---|
| **Cameră** | număr (ID unic), tip (`SINGLE`/`DOUBLE`/`SUITE`/`PENTHOUSE`), preț/noapte (RON), capacitate maximă |
| **Client** | CNP (13 cifre), nume, prenume, email, telefon |
| **Cerere rezervare** | tipul camerei dorite, zi check-in (1–365), zi check-out (1–365), număr persoane |

### Operații posibile

1. `adaugaCamera()` — adaugă o cameră în vectorul fix al hotelului (maxim 50)
2. `esteValabila()` — verifică dacă o cameră este liberă într-un interval, detectând suprapunerile prin condiția `!(checkin >= ziCheckout || checkout <= ziCheckin)`
3. `cereRezervare()` — caută prima cameră de tipul dorit, liberă și cu capacitate suficientă, calculează prețul total și înregistrează rezervarea

---

## Structura Proiectului
```
hotel.cpp       ← fișier sursă unic, conține toate clasele și main()
```

---

## Clasele Proiectului

### `Camera`
Reprezintă o cameră de hotel. Câmpul `tip` este `char*` alocat dinamic, gestionat prin funcția privată `copiazaTip()`.

### `Client`
Datele personale ale unui client. Toate câmpurile (`cnp`, `nume`, `prenume`, `email`, `telefon`) sunt `char*` alocate dinamic. Funcțiile private `copiazaSir()`, `copiazaDin()` și `elibereaza()` centralizează managementul memoriei.

### `Rezervare`
Leagă un obiect `Client` de un obiect `Camera` pe un interval de zile. Conține prin **compunere** câte un obiect `Client` și `Camera`. Numărul de nopți se calculează la cerere: `zi_checkout - zi_checkin`.

### `Hotel`
Gestionează un **vector static** de maxim 50 de camere și un **vector dinamic** de rezervări (redimensionabil automat prin dublare). Conține logica de business a sistemului.

---

## Funcționalități Principale

- **`esteValabila(nrCamera, checkin, checkout)`** — parcurge toate rezervările active și verifică suprapunerea cu intervalul cerut. Camera nu are un câmp boolean „ocupat" — devine indisponibilă strict prin existența unei rezervări în vector.

- **`cereRezervare(client, tipDorit, checkin, checkout, nrpers)`** — caută prima cameră de tipul dorit cu capacitate suficientă și liberă în intervalul solicitat, calculează `preț = nopți × preț/noapte` și înregistrează rezervarea, blocând camera pentru acea perioadă.

- **Redimensionare dinamică** — vectorul de rezervări se dublează automat prin `extindeRezervari()` când capacitatea este atinsă.

---

## Scenarii de Test (din `main`)

| # | Scenariu | Rezultat așteptat |
|---|---|---|
| 1 | Rezervări normale pentru fiecare tip de cameră | Confirmate |
| 2 | Rezervare cu suprapunere pe camera 101 → se alocă 102 | Redirect automat |
| 3 | Singura SUITE ocupată; se cere aceeași perioadă | Eșec — nicio cameră disponibilă |
| 4 | `checkin=100`, `checkout=50` (interval invalid) | Eșec — interval invalid |
| 5 | SINGLE cu 3 persoane (depășește capacitatea) | Eșec — capacitate insuficientă |
| 6 | Rezervare imediat după interval ocupat (`checkin=153`) | Confirmat — fără suprapunere |
| 7 | 0 persoane și tip inexistent (`VILLA`) | Eșec — input invalid |

---

## Cerințe Atinse

- [x] Descrierea problemei în comentariu (date de intrare + operații posibile)
- [x] Minim 3 clase specifice domeniului — `Camera`, `Client`, `Rezervare`, `Hotel`
- [x] Constructor de inițializare cu parametri pentru fiecare clasă
- [x] Constructor de copiere pentru fiecare clasă
- [x] `operator=` de copiere pentru fiecare clasă
- [x] Destructor pentru fiecare clasă
- [x] `operator<<` pentru toate clasele, cu compunere de apeluri
- [x] Utilizare extensivă a `const` (getteri, parametri, referințe) și funcții private
- [x] Minim 3 funcții publice netriviale — `adaugaCamera`, `esteValabila`, `cereRezervare`
- [x] Cel puțin 1–2 funcții complexe — `esteValabila` și `cereRezervare`
- [x] Scenariu de utilizare cu sens în `main`
- [x] Date reprezentative generate la pornire (7 camere + 4 clienți, fără fișiere externe)
- [x] Acoperirea mai multor ramuri de execuție prin 7 teste automate
