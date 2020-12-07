#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>  
#include <functional>
using namespace std::placeholders;

struct Polaczenie
{
    unsigned dzien;
    std::string nr;
    float czas;
    std::string kod () const
    {
        return nr.substr(0,2);
    }
};



class Histogram
{
    public:
        void dodaj (float x)
        {
            dane_.push_back(x);
        }
        size_t rozmiar () const
        {
            return dane_.size();
        }
        float srednia () const
        {
            if(srednia_akt_ == true)
            {
                return srednia_;
            }
            else
            {
                float srednia = 0;
                std::for_each(dane_.begin(), dane_.end(), [&srednia] (auto &x) {srednia += x;} );
                srednia /= dane_.size();
                srednia_akt = true;
                srednia_ = srednia;
                return srednia;
            }
                
        }
        float odchylenie () const
        {
            if(odchylenie_akt_ == true)
            {
                return odchylenie_;
            }
            else
            {
                float suma = 0;
                float odchylenie = 0;
                std::for_each(dane_.begin(), dane_.end(), [&suma] (auto &x) {suma += x*x;} );
                odchylenie = sqrt((suma/dane_.size())-srednia_);
                odchylenie_akt_ = true;
                odchylenie_ = odchylenie;
                return odchylenie;
            }
        }

        float max () const
        {
            return *std::max_element(dane_.begin(), dane_.end());
        }
        float min () const
        {
            return *std::min_element(dane_.begin(), dane_.end());
        }
    private:
        std::vector<float> dane_;
        mutable bool srednia_akt_;
        mutable bool odchylenie_akt_;
        mutable float srednia_;
        mutable float odchylenie_;    
};


std::istream &operator>> (std::istream &is, Polaczenie &p)
{
    is >> p.dzien >> p.nr >> p.czas;
    return is;
}

class Billing
{
    public:
        explicit Billing(std::istream &is)
        {
            std::string nazwa_pliku;
            is >> nazwa_pliku;
            Polaczenie p;
            std::ifstream myfile(nazwa_pliku);
            while(myfile >> p.dzien >>p.nr >>p.czas)
            {
                blng_.push_back(p);
            } 
            myfile.close();
        }  
        // explicit Billing(std::istream &is)
        // {
        //     Polaczenie p;
        //     while(is >> p)
        //     {
        //         blng_.push_back(p);
        //     } 
        // }  

        void statystykaDzienna (std::ostream &os) const
        {
            std::vector <unsigned> stat (31, 0);
            for(int i = 0; i<blng_.size(); i++)
            {   
                for(int j = 0; j<stat.size(); j++)
                {
                    if(blng_[i].dzien == j+1)
                    {
                        stat[j] ++;
                    }
                }
            }

            int suma_poloczen = 0;
            for(int j = 0; j<stat.size(); j++)
            {
                suma_poloczen += stat[j];
            }

            std::cout<<"*** STATYSTYKA DZIENNA ***"<<std::endl;
            for(int i = 0; i<stat.size(); i++)
            {
                float procent = 100*((float)stat[i]/(float)suma_poloczen);
                os<<i+1<<":"<<"\t"<<stat[i]<<" "<<"("<<procent<<"%):"<<"\t"; //ustawic flagami
                int gwiazdki = (int)procent*5; //to mnonzenie to po to by wicej gwaizdek bylo 
                for(int j = 0; j<gwiazdki; j++)
                {
                    std::cout<<"*";
                }
                std::cout<<std::endl;
            }

            std::cout<<"WYKONANYCH POLACZEN: "<<suma_poloczen<<std::endl;
        }

        void statystykaKrajowa (std::ostream &os) const
        {
            std::map <std::string, Histogram> stat;
            for(int i = 0; i<blng_.size(); i++)
            {
                if(stat.count((std::string)blng_[i].kod())<1)
                {
                    Histogram histogram;
                    stat [(std::string)blng_[i].kod()] = histogram;
                }
 
                stat [(std::string)blng_[i].kod()].dodaj((float)blng_[i].czas);
            }

            std::cout<<"\n"<<"*** STATYSTYKA KRAJOWA ***"<<"\n"<<"Kraj\tN\tSred.\tOdch.\tMin\tMax" <<std::endl;
            for(std::map<std::string, Histogram>::const_iterator it = stat.begin(); it != stat.end(); ++it)
            {
                std::cout<<it->first<<"\t"<<it -> second.rozmiar()<<"\t"<<it -> second.srednia()<<"\t"<<it -> second.odchylenie()<<"\t"<<it -> second.min()<<"\t"<<it -> second.max()<<std::endl;
            }
        }


    private:
        std::vector <Polaczenie> blng_;

};

int main()
{
    Billing bill (std::cin);
    bill.statystykaDzienna(std::cout);
    bill.statystykaKrajowa(std::cout);
    return 0;
}