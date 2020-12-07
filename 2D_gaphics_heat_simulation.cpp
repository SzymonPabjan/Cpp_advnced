#include <allegro.h>
#include <stdio.h>
#include <iostream>
#include <err.h>
#include <vector>
#include <algorithm>
#include <math.h>

const double c = 440;
const double m = 100;
double MAX_TEMP = 16000.0;
int LICZBA_CZESCI_GLOBAL = 400;
int WIELKOSC_PALNIKA_GLOBAL = 5;
int MOC_PALNIKA_GLOBAL = 10000;
int Y_OKNO_GLOBAL = 1000;
int X_OKNO_GLOBAL = 800;

int przerwanie_klawiatury(int k)
{
	if((k & 0xff) == '1')
	{
		WIELKOSC_PALNIKA_GLOBAL = 1;
	}
	else if((k & 0xff) == '2')
	{
		WIELKOSC_PALNIKA_GLOBAL = 2;
	}
	else if((k & 0xff) == '2')
	{
		WIELKOSC_PALNIKA_GLOBAL = 5;
	}
	else if((k & 0xff) == '3')
	{
		WIELKOSC_PALNIKA_GLOBAL = 10;
	}
	else if((k & 0xff) == '4')
	{
		WIELKOSC_PALNIKA_GLOBAL = 20;
	}
	else if((k & 0xff) == '5')
	{
		WIELKOSC_PALNIKA_GLOBAL = 30;
	}
	else if((k & 0xff) == '6')
	{
		WIELKOSC_PALNIKA_GLOBAL = 40;
	}
	else if((k & 0xff) == '7')
	{
		WIELKOSC_PALNIKA_GLOBAL = 50;
	}
	else if((k & 0xff) == '8')
	{
		WIELKOSC_PALNIKA_GLOBAL = 70;
	}
	else if((k & 0xff) == '9')
	{
		WIELKOSC_PALNIKA_GLOBAL = 100;
	}
	else if((k & 0xff) == 'q')
	{
		if(MOC_PALNIKA_GLOBAL <= 10000)
		{
			MOC_PALNIKA_GLOBAL = 1000000;
		}
		else
		{
			MOC_PALNIKA_GLOBAL -= 10000;
		}
		
	}
	else if((k & 0xff) == 'e')
	{
		if(MOC_PALNIKA_GLOBAL >= 1000000)
		{
			MOC_PALNIKA_GLOBAL = 10000;
		}
		else
		{
			MOC_PALNIKA_GLOBAL += 10000;
		}
	}
	
	return 0;
	
}
END_OF_FUNCTION(przerwanie_klawiatury)
void palnik(std::vector<std::vector<double>> &T_arr, int x_pos, int y_pos, double moc, int zasieg, double masa_kawalka)
{
	double delta_T = (0.03 * moc)/(c*masa_kawalka*zasieg*zasieg);
	int przesuniecie = (int)(floor(zasieg/2));
	for(int i = 0; i<zasieg; i++)
	{
		for(int j = 0; j<zasieg; j++)
		{
			if(!(x_pos+(i-przesuniecie) < 0) && !(x_pos+(i-przesuniecie)>799) && !(y_pos+(j-przesuniecie) < 0) && !((y_pos+(j-przesuniecie)>799)))
			{
				T_arr[x_pos+(i-przesuniecie)][y_pos+(j-przesuniecie)] += delta_T; 
			}
			
		}
	}
}

void kolor(int &red, int &green, int &blue, int Temp_int)
{
	if(Temp_int <=66)
	{
		red = 255;
	green = Temp_int;
		green = (int)(99.471 *log(green) - 161.12);
		if(Temp_int <=19)
		{
			blue = 0;
		}
		else
		{
			blue = Temp_int-10;
			blue = (int)(138.52 *log(blue) - 305.04);
		}
	}
	else
	{
		red = Temp_int - 60;
		red = (int)(288*pow(red, -0.133));
		green = Temp_int - 60;
		green = (int)(288.12*pow(green, - 0.0756));
		blue = 255;
	}
	
}


void licz_i_rysuj(std::vector<std::vector<double>> &T_arr, std::vector<std::vector<double>> &T_arr_new, const int &liczba_czesci, const double &masa_kawalka, int &i)
{
	for(int i = 0; i<liczba_czesci; i++)
		{
			for(int j = 0; j<liczba_czesci; j++)
			{
				if(true)
				{
					double tymczasowa_suma = T_arr[i][j];
					if(i != 0)
					{
						tymczasowa_suma -= 0.03 * (1/(c*masa_kawalka)) * (T_arr[i][j] - T_arr[i-1][j]);
					}
					if(i != liczba_czesci-1)
					{
						tymczasowa_suma -= 0.03 * (1/(c*masa_kawalka)) * (T_arr[i][j] - T_arr[i+1][j]);
					}
					if(j != 0)
					{
						tymczasowa_suma -=  0.03 * (1/(c*masa_kawalka)) * (T_arr[i][j] - T_arr[i][j-1]);
					}
					if(j != liczba_czesci-1)
					{
						tymczasowa_suma -= 0.03 * (1/(c*masa_kawalka)) * (T_arr[i][j] - T_arr[i][j+1]);
					}
					
					T_arr_new[i][j] = tymczasowa_suma;
					if(T_arr_new[i][j] >= MAX_TEMP)
					{
						T_arr_new[i][j] = MAX_TEMP;
					}
				
				}
			}
		}
		for(int i = 0; i<liczba_czesci; i++)
		{
			for(int j = 0; j<liczba_czesci; j++)
			{
				T_arr[i][j] = T_arr_new[i][j];
				T_arr_new[i][j] = 0;
			}
		}
		if(i%5 == 0)
		{
			BITMAP *bmp_outside = create_bitmap(800, 800);
			for(int i = 0; i<liczba_czesci; i++)
			{
				for(int l = 0; l<liczba_czesci; l++)
				{
					for(int k = 0; k<(800/liczba_czesci); k++)
					{
						for(int j = 0; j<(800/liczba_czesci); j++)
						{
								int red = 0;
								int green = 0;
								int blue = 0;
								int Temp_int = (int)(T_arr[i][l]/100);
								kolor(red, green, blue, Temp_int);
								putpixel(bmp_outside, (((800/liczba_czesci)*i) + k), (((800/liczba_czesci)*l)+j), makecol(red, green, blue));
						}
					}
				}
			}
			BITMAP *bmp_interface_1 = create_bitmap(800, 50);
			BITMAP *bmp_interface_2 = create_bitmap(800, 50);
			BITMAP *bmp_interface_3 = create_bitmap(800, 50);
			int pos = mouse_pos;
			int x_pos_scale = (int)(floor((liczba_czesci * (pos >> 16))/800));
			int y_pos_scale = (int)(floor((liczba_czesci * (pos & 0x0000ffff))/800));
			std::string interface_tresc_1 = "Wielkosc palnika: " + std::to_string(WIELKOSC_PALNIKA_GLOBAL); 
			std::string interface_tresc_2 = "Moc palnika: " + std::to_string(MOC_PALNIKA_GLOBAL/1000); 
			std::string interface_tresc_3 = "Temp. pixela: " + std::to_string((float)T_arr[x_pos_scale][y_pos_scale]); 
			textprintf_centre_ex(bmp_interface_1, font, 400, 25, makecol(255, 255, 255), makecol(0,0, 0) , interface_tresc_1.c_str());
			textprintf_centre_ex(bmp_interface_2, font, 400, 25, makecol(255, 255, 255), makecol(0,0, 0) , interface_tresc_2.c_str());
			textprintf_centre_ex(bmp_interface_3, font, 400, 25, makecol(255, 255, 255), makecol(0,0, 0) , interface_tresc_3.c_str());
			blit(bmp_interface_1, screen, 0,0,0,800, 800, 50);
			blit(bmp_interface_2, screen, 0,0,0,850, 800, 50);
			blit(bmp_interface_3, screen, 0,0,0,900, 800, 50);
			blit(bmp_outside, screen, 0, 0, 0, 0, 800, 800);
			destroy_bitmap(bmp_interface_1);
			destroy_bitmap(bmp_interface_2);
			destroy_bitmap(bmp_interface_3);
			destroy_bitmap(bmp_outside);
		}
}

void rysuj()
{

	static std::vector<std::vector<double>> T_arr;
	static std::vector<std::vector<double>> T_arr_new;
	for(int k = 0; k<LICZBA_CZESCI_GLOBAL; k++)
	{
		std::vector<double> wektor;
		T_arr.push_back(wektor);
		T_arr_new.push_back(wektor);
		for(int l = 0; l < LICZBA_CZESCI_GLOBAL; l++)
		{
			T_arr[k].push_back(293);
			T_arr_new[k].push_back(293);
		}
	}
	const int liczba_czesci = LICZBA_CZESCI_GLOBAL;
	const double masa_kawalka = m/(liczba_czesci*liczba_czesci);
	int i = 0;
	LOCK_FUNCTION(przerwanie_klawiatury);
	while(1)
	{
		keyboard_callback = przerwanie_klawiatury;
		licz_i_rysuj(T_arr, T_arr_new, liczba_czesci, masa_kawalka, i);
		while(mouse_b &1)
		{
			int pos = mouse_pos;
			int x_pos = pos >> 16;
			int y_pos =  pos & 0x0000ffff;
			int x_pos_scale = (int)(floor((liczba_czesci * x_pos)/800));
			int y_pos_scale = (int)(floor((liczba_czesci * y_pos)/800));
			palnik(T_arr, x_pos_scale, y_pos_scale , MOC_PALNIKA_GLOBAL, WIELKOSC_PALNIKA_GLOBAL, masa_kawalka);
			licz_i_rysuj(T_arr, T_arr_new, liczba_czesci, masa_kawalka, i);
			sleep(0.001);
			i++;
		}
		
		if(mouse_b & 2)
		{
			break;
		}
		sleep(0.001);
		i++;
	}
}


int main(void)
{
	allegro_init();
	install_timer();
	install_keyboard();
	install_mouse();
	
	set_color_depth(16);
	if(set_gfx_mode (GFX_AUTODETECT_WINDOWED, X_OKNO_GLOBAL, Y_OKNO_GLOBAL, 0, 0) != 0)
	{
		std::cerr<<"Blad";
		return 1;
	}
	
	set_window_title ("Symulator spawacza");
	show_mouse(screen);
	

	rysuj();
		
	std::cout<<"Zamykanie okna!"<<std::endl;
	sleep(1);
	allegro_exit();
	
	
	return 0;
}
END_OF_MAIN()


