#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <map>
#include <math.h>  
#include <algorithm>

class DaneStat 
{
    public:
        DaneStat (const std::string &nazwa)
        {
            nazwa_ = nazwa;
        }
        virtual const std::vector <float> &dane () const = 0;
        virtual ~DaneStat () {};
        virtual const std::string &nazwa () const 
        {
            return nazwa_;
        }

    protected:
        std::string nazwa_;
};

class DaneStatReal : public DaneStat
{
    public:
        DaneStatReal(const std::string &nazwa) : DaneStat(nazwa)
        {
            nazwa_ = nazwa;
        }

        const std::vector <float> &dane () const
        {
            std::ifstream myfile(nazwa_);
            std::string linijka;
            while(myfile >> linijka)
            {
                wczytane_dane_.push_back(std::stof(linijka));
            }
            myfile.close();
            return wczytane_dane_;
        }

    private:
        mutable std::vector <float> wczytane_dane_;
};

class DaneStatProxy : public DaneStat
{
    private:
        mutable DaneStat *dane_ = NULL;
        std::string nazwa_;
    public:
        DaneStatProxy(const std::string &nazwa) : DaneStat(nazwa)
        {
            nazwa_ = nazwa;
        }

        ~DaneStatProxy() {delete dane_;}
        virtual const std::vector <float> &dane () const
        {
            if(!dane_)
            {
                dane_ = new DaneStatReal(nazwa_);
            }
           return dane_ -> dane();
        }  
};

typedef std::map <std::string, float> ParametryRozkladu;

class Rozklad
{
    public:
        explicit Rozklad (const std::vector <float> &dane) : dane_(dane) {}
        virtual ~Rozklad () {}
        virtual std::unique_ptr <ParametryRozkladu> oblicz () const = 0;
    protected:
        const std::vector <float> &dane_;
};

class RozkladGaussa : public Rozklad
{
    public:
        explicit RozkladGaussa (const std::vector <float> &dane) : Rozklad(dane) {}
        virtual ~RozkladGaussa () {}
        virtual std::unique_ptr <ParametryRozkladu> oblicz () const
        {
            unsigned int dlugosc = dane_.size();
            float srednia = 0;
            std::for_each(dane_.begin(), dane_.end(), [&srednia] (auto &x) {srednia += x;} );
            srednia /= dlugosc;

            float suma = 0;
            float odchylenie = 0;
            std::for_each(dane_.begin(), dane_.end(), [&suma] (auto &x) {suma += x*x;} );
            odchylenie = sqrt((suma/dlugosc)-srednia);

            std::cout<<"Srednia: "<<srednia<<"\t"<<"Odchylenie: "<<odchylenie<<std::endl;

            std::string dlugosc_str = std::to_string(dlugosc);
            std::string srednia_str = "Gauss srednia dla " + dlugosc_str;
            std::string odchylenie_str = "Gauss odchylenie dla " + dlugosc_str;
            std:auto pointer = std::make_unique <ParametryRozkladu> ();
            pointer -> insert(std::pair<std::string, float>(srednia_str, srednia));
            pointer -> insert(std::pair<std::string, float>(odchylenie_str, odchylenie));

            return pointer;
        }
        static Rozklad* kreator (const std::vector <float> &dane)
        {
            return new RozkladGaussa (dane);
        }
};

class RozkladLorentza : public Rozklad
{
    public:
        explicit RozkladLorentza (const std::vector <float> &dane) : Rozklad(dane) {}
        virtual ~RozkladLorentza () {}
        virtual std::unique_ptr <ParametryRozkladu> oblicz () const
        {

            unsigned int dlugosc = dane_.size();
            float srednia = 0;
            std::for_each(dane_.begin(), dane_.end(), [&srednia] (auto &x) {srednia += x;} );
            srednia /= dlugosc;

            float mediana = 0;
            std::vector <float> posortowany = dane_;
            std::sort(posortowany.begin(), posortowany.end());
            if(dlugosc%2 == 0)
            {
                mediana = (posortowany[(dlugosc/2)-1] + posortowany[dlugosc/2])/2;
            }
            else
            {
                mediana = posortowany[dlugosc/2];
            }

            std::cout<<"Srednia: "<<srednia<<"\t"<<"Mediana: "<<mediana<<std::endl;

            std::string dlugosc_str = std::to_string(dlugosc);
            std::string srednia_str = "Lorentz srednia dla " + dlugosc_str;
            std::string mediana_str = "Lorentz mediana dla " + dlugosc_str;
            std:auto pointer = std::make_unique <ParametryRozkladu> ();
            pointer -> insert(std::pair<std::string, float>(srednia_str, srednia));
            pointer -> insert(std::pair<std::string, float>(mediana_str, mediana));

            return pointer;
        }
        static Rozklad* kreator (const std::vector <float> &dane)
        {
            return new RozkladLorentza (dane);
        }
};

class RozkladPoissona : public Rozklad
{
    public:
        explicit RozkladPoissona (const std::vector <float> &dane) : Rozklad(dane) {} 
        virtual ~RozkladPoissona () {}
        virtual std::unique_ptr <ParametryRozkladu> oblicz () const
        {
            unsigned int dlugosc = dane_.size();
            float srednia = 0;
            std::for_each(dane_.begin(), dane_.end(), [&srednia] (auto &x) {srednia += x;} );
            srednia /= dlugosc;

            std::cout<<"Srednia: "<<srednia<<std::endl;

            std::string dlugosc_str = std::to_string(dlugosc);
            std::string srednia_str = "Poisson srednia dla " + dlugosc_str;
            std:auto pointer = std::make_unique <ParametryRozkladu> ();
            pointer -> insert(std::pair<std::string, float>(srednia_str, srednia));
            return pointer;
        }
        static Rozklad* kreator (const std::vector <float> &dane)
        {
            return new RozkladPoissona (dane);
        }
};

typedef Rozklad* (*KreatorRozkladu)(const std::vector <float> &); 

class FabrykaRozkladow 
{
    private:
        static std::map <unsigned, KreatorRozkladu> rozklady;
        static std::map <unsigned, std::string> nazwy;
    public:

        // rejestruje kreator danego rozkladu (id generowane przyrostowo od 1)
        static void rejestruj (KreatorRozkladu kr, const std::string &nazwa)
        {
            unsigned id = FabrykaRozkladow::ilosc() + 1;
            rozklady [id] = kr;
            nazwy [id] = nazwa;
        }
        // wola kreator dla rozkladu o wybranym id
        static Rozklad *utworz (unsigned id, const std::vector <float> &dane)
        {
            return rozklady [id] (dane);
        }
        static std::string nazwa (unsigned id)
        {
            return nazwy [id];
        }
        static unsigned ilosc () {return rozklady.size ();}
};

std::map <unsigned, KreatorRozkladu> FabrykaRozkladow::rozklady;
std::map <unsigned, std::string> FabrykaRozkladow::nazwy;

int main(int argc, char** argv)
{

    std::vector <std::shared_ptr <DaneStat> > dane;
    for (int i = 1; i < argc; ++i)
    {
        auto dane_nazwy = std::make_shared <DaneStatProxy> (argv[i]);
        dane.push_back(dane_nazwy);
    } 

    FabrykaRozkladow::rejestruj(&RozkladGaussa::kreator, std::string("rozklad gaussa"));
    FabrykaRozkladow::rejestruj(&RozkladLorentza::kreator, std::string("rozklad lorentza"));
    FabrykaRozkladow::rejestruj(&RozkladPoissona::kreator, std::string("rozklad poissona"));
    while(true)
    {
        std::cout<<"Lista wczytanych plkow:"<<std::endl;
        for (int i = 1; i < argc; ++i)
        {
            std::cout<<i<<". "<<argv[i]<<std::endl;
        } 

        std::cout<<"Wybierz z listy plik do analizy wpisujac jego index lub wpisz 0 aby zakonczyc program"<<std::endl;
        int akcja_uzytkownika = 0;
        std::cin>> akcja_uzytkownika;
        if(akcja_uzytkownika == 0)
        {
            std::cout<<std::endl<<"Wychodznie z programu"<<std::endl;
            break;
        }
        else
        {
            std::cout<<"Wybrano plik "<<argv[akcja_uzytkownika]<<std::endl<<"Wybierz dla jakiego rozkladu wykonac obliczenia: "<<std::endl;
            std::cout<<"1. Gaussa"<<std::endl<<"2. Lorentza"<<std::endl<<"3. Poissona"<<std::endl;
            int akcja_uzytkownika_2 = 0;
            std::cin>> akcja_uzytkownika_2;
            std::cout<<"Obliczam"<<std::endl;
            
            std::unique_ptr <Rozklad> rozkl (FabrykaRozkladow::utworz (akcja_uzytkownika_2, dane[akcja_uzytkownika-1]->dane ()));
            rozkl -> oblicz();   
            std::cout<<std::endl;         
        }

    }

    return 0;
}