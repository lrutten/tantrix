#include <iostream>
#include <cmath>
#include <memory>
#include <array>
#include <vector>
#include <optional>
#include <concepts>
#include <coroutine>
#include <exception>
#include <utility>

#include <QApplication>
#include <QPushButton>
#include <QPainter>

constexpr int n_tegels  = 10;
constexpr int bordsize  = 6;  // was 12
constexpr int startr    = 3;  // bordsize/2;
constexpr int startk    = 3;  //bordsize/2;
constexpr int zijden    = 6;
constexpr int tegel_br  = 100;
constexpr int boord     = tegel_br/2;
constexpr int veld_br   = bordsize * tegel_br + tegel_br/2 + 2*boord;
const double straal     = tegel_br/2.0/cos(30.0/360*2.0*std::numbers::pi);
constexpr int boogdikte = 10;

// De 6 punten van de Plaats in Plaatscoordinaten
const double xx0 = 0.0;
const double yy0 = -straal;
const double xx1 = tegel_br/2.0;
const double yy1 = -straal/2.0;
const double xx2 = tegel_br/2.0;
const double yy2 = straal/2.0;
const double xx3 = 0.0;
const double yy3 = straal;
const double xx4 = -tegel_br/2.0;
const double yy4 = straal/2.0;
const double xx5 = -tegel_br/2.0;
const double yy5 = -straal/2.0;

enum kleur_t
{
  R = 0,
  G = 1,
  B = 2
};

enum richting_t
{
   NO = 0,
   O  = 1,
   ZO = 2,
   ZW = 3,
   W  = 4,
   NW = 5 
};

constexpr int n_kleuren    = 3;
constexpr int n_richtingen = zijden;

// Calculate the oppsite side
int opposite(int ri)
{
   ri += zijden/2;
   if (ri >= zijden)
   {
      ri -= zijden;
   }
   return ri;
}

class BoogFout
{
};

class Tegel
{
private:
   kleur_t kleur;
   int nummer;
   std::array<kleur_t, zijden>                    kleuren;
   std::array<std::vector<richting_t>, n_kleuren> bogen;

public:
   Tegel(kleur_t hfd, int nr, kleur_t kl0, kleur_t kl1, kleur_t kl2, kleur_t kl3, kleur_t kl4, kleur_t kl5 );
   Tegel(const Tegel &van);
   int getBegin(kleur_t kl)
   {
      return bogen[kl][0];
   }
   int getEnd(kleur_t kl)
   {
      return bogen[kl][1];
   }
   kleur_t get_kleur()
   {
      return kleur;
   }
   kleur_t get_kleuren(richting_t ri)
   {
      return kleuren[ri];
   }
};

Tegel::Tegel(kleur_t hfd, int nr, kleur_t kl0, kleur_t kl1, kleur_t kl2, kleur_t kl3, kleur_t kl4, kleur_t kl5) :
   kleur(hfd), nummer(nr)
{
   kleuren[0] = kl0;   
   kleuren[1] = kl1;   
   kleuren[2] = kl2;   
   kleuren[3] = kl3;   
   kleuren[4] = kl4;   
   kleuren[5] = kl5;
   
   for (int i=NO; i<n_richtingen; i++)
   {
      int kl = kleuren[i];
      bogen[kl].push_back((richting_t)i);
   }
   
   for (int i=0; i<n_kleuren; i++)
   {
      if (bogen[i].size() != 2)
      {
         throw BoogFout();
      }
      
      if (bogen[i][1] - bogen[i][0] > 3)
      {
         // corrigeer de afstand, max 3
         int r0 = bogen[i][0];
         int r1 = bogen[i][1];
         r0 += n_richtingen;
         
         // wissel
         int t = r0;
         r0 = r1;
         r1 = t;
         
         bogen[i][0] = (richting_t)r0;
         bogen[i][1] = (richting_t)r1;
      }

      // check
      if (bogen[i][1] - bogen[i][0] > 3)
      {
         throw BoogFout();
      }
   }
}

// ok
Tegel::Tegel(const Tegel &van) :
   kleur(van.kleur),
   nummer(van.nummer),
   kleuren(van.kleuren),
   bogen(van.bogen)
{
}
   

class Plaats
{
private:
   std::unique_ptr<Tegel> tegel;
   int                    hoek;
   std::array<std::weak_ptr<Plaats>, zijden> buren;
   int                    r;
   int                    k;

public:
   Plaats();
   Plaats(const Plaats &van);
   void toon();
   void zet_buur(richting_t ri, std::shared_ptr<Plaats> buur);
   std::shared_ptr<Plaats> get_buur(richting_t ri);
   int aantal_buren();
   void zet_tegel(std::unique_ptr<Tegel> tgl);
   std::unique_ptr<Tegel> haalop_tegel();
   bool bezet();
   kleur_t get_kleur(richting_t ri);
   void zet_hoek(int ho)
   {
      hoek = ho;
   }
   void inc_hoek()
   {
      hoek++;
   }
   void set_rk(int rr, int kk)
   {
      r = rr;
      k = kk;
   }
   std::tuple<int, int> get_rk()
   {
      return {r, k};
   }
   void teken(QPainter &painter);
   void boog1(QPainter &painter, kleur_t kleur);
   void boog2(QPainter &painter, kleur_t kleur);
   void boog3(QPainter &painter, kleur_t kleur);
   void roteer(QPainter &painter, int h);
};

Plaats::Plaats() : tegel(nullptr), hoek(0), r(0), k(0)
{
}

// ok
Plaats::Plaats(const Plaats &van) : 
   hoek(van.hoek), r(van.r), k(van.k)
{
   if (van.tegel != nullptr)
   {
      tegel = std::make_unique<Tegel>(*van.tegel);
   }
}

void Plaats::toon()
{
   std::cout << "*";
}

void Plaats::zet_buur(richting_t ri, std::shared_ptr<Plaats> buur)
{
   buren[ri] = buur;
}

std::shared_ptr<Plaats> Plaats::get_buur(richting_t ri)
{
   return buren[ri].lock();
}

void Plaats::zet_tegel(std::unique_ptr<Tegel> tgl)
{
   tegel = std::move(tgl);
}

std::unique_ptr<Tegel> Plaats::haalop_tegel()
{
   return std::move(tegel);
}

bool Plaats::bezet()
{
   return tegel != nullptr;
}

kleur_t Plaats::get_kleur(richting_t ri)
{
   if (bezet())
   {
      int ri2 = ri - hoek;
      if (ri2 < 0)
      {
         ri2 += zijden;
      }
      return tegel->get_kleuren((richting_t)ri2);
   }
   else
   {
      return R; // rood
   }
}

int Plaats::aantal_buren()
{
   int n = 0;
   for (int ri=0; ri<zijden;ri++)
   {
      std::shared_ptr<Plaats> buur = get_buur((richting_t)ri);
      if (buur != nullptr && buur->bezet())
      {
         n++;
      }
   }
   return n;
}

QColor naarQColor(kleur_t kleur)
{
   switch (kleur)
   {
      case R:
         return Qt::red;
         break;
      case G:
         return Qt::yellow;
         break;
      case B:
         return Qt::blue;
         break;
      default:
         return Qt::blue;
         break;
   }
}

void Plaats::roteer(QPainter &painter, int h)
{
   painter.save();
   painter.rotate(60.0 * h);
}

void Plaats::boog1(QPainter &painter, kleur_t kleur)
{
   QPen pen;
   pen.setWidth(boogdikte);
   pen.setColor(naarQColor(kleur));
   painter.setPen(pen);

   // boog1, van een zijde naar zijde 1 verder
   painter.drawArc(xx1-straal/2, yy1-straal/2, straal, straal, 16*150, 16*120);
}

void Plaats::boog2(QPainter &painter, kleur_t kleur)
{
   QPen pen;
   pen.setWidth(boogdikte);
   pen.setColor(naarQColor(kleur));
   painter.setPen(pen);

   // boog2, van een zijde naar een zijde 2 verder
   double bc = straal;
   double ac = straal/sin(30.0/360.0*2.0*std::numbers::pi);
   double ab = ac*cos(30.0/360.0*2.0*std::numbers::pi);
   double boogr = ac - straal/2;
   //std::cout << "bc " << bc << "\n";
   //std::cout << "ac " << ac << "\n";
   //std::cout << "ab " << ab << "\n";
   painter.drawArc(ab - boogr, -boogr, 2*boogr, 2*boogr, 16*150, 16*60); 
}

void Plaats::boog3(QPainter &painter, kleur_t kleur)
{
   QPen pen;
   pen.setWidth(boogdikte);
   pen.setColor(naarQColor(kleur));
   painter.setPen(pen);

   // lijn, van een zijde naar zijde 3 verder
   painter.drawLine(-tegel_br/2, 0, tegel_br/2, 0);
}

void Plaats::teken(QPainter &painter)
{
   //std::cout << "Plaats::teken\n";

   // teken de rand met een zwarte lijn
   QPen pen;
   pen.setWidth(3);
   pen.setColor(Qt::black);
   painter.setPen(pen);
   
   painter.drawLine( xx0, yy0, xx1, yy1);
   painter.drawLine( xx1, yy1, xx2, yy2);
   painter.drawLine( xx2, yy2, xx3, yy3);
   painter.drawLine( xx3, yy3, xx4, yy4);
   painter.drawLine( xx4, yy4, xx5, yy5);
   painter.drawLine( xx5, yy5, xx0, yy0);

   // toon de buren met een blauwe lijn
   //  enkel voor de test
   /*
   QPen pen2;
   pen2.setWidth(3);
   pen2.setColor(Qt::blue);
   painter.setPen(pen2);

   double x_no = (xx0+xx1)/2.0;
   double y_no = (yy0+yy1)/2.0;
   double x_o  = (xx1+xx2)/2.0;
   double y_o  = (yy1+yy2)/2.0;
   double x_zo = (xx2+xx3)/2.0;
   double y_zo = (yy2+yy3)/2.0;
   double x_zw = (xx3+xx4)/2.0;
   double y_zw = (yy3+yy4)/2.0;
   double x_w  = (xx4+xx5)/2.0;
   double y_w  = (yy4+yy5)/2.0;
   double x_nw = (xx5+xx0)/2.0;
   double y_nw = (yy5+yy0)/2.0;

   if (buren[NO] != nullptr)
   {
      painter.drawLine(0, 0, x_no, y_no);
   }
   if (buren[O] != nullptr)
   {
      painter.drawLine(0, 0, x_o, y_o);
   }
   if (buren[ZO] != nullptr)
   {
      painter.drawLine(0, 0, x_zo, y_zo);
   }
   if (buren[ZW] != nullptr)
   {
      painter.drawLine(0, 0, x_zw, y_zw);
   }
   if (buren[W] != nullptr)
   {
      painter.drawLine(0, 0, x_w, y_w);
   }
   if (buren[NW] != nullptr)
   {
      painter.drawLine(0, 0, x_nw, y_nw);
   }
    */
   if (tegel != nullptr)
   {
      //std::cout << "teken met tegel\n";
      static const QPoint zeshoek[6] = 
      {
         QPoint(xx0, yy0),
         QPoint(xx1, yy1),
         QPoint(xx2, yy2),
         QPoint(xx3, yy3),
         QPoint(xx4, yy4),
         QPoint(xx5, yy5)
      };
      
      // teken de achtergrond van de tegel
      painter.setPen(Qt::NoPen);
      painter.setBrush(Qt::green);
      painter.drawConvexPolygon(zeshoek, 6);
      
      // teken een voorlopige boog
      // deze boog raakt de 6 zijden van de zeshoek 
      // voor de test
      /*
      QPen pen3;
      pen3.setWidth(boogdikte);
      pen3.setColor(Qt::red);
      painter.setPen(pen3);
      painter.drawArc(-tegel_br/2, -tegel_br/2, tegel_br, tegel_br, 16*45, 16*270); 
       */
      
      // voor de test
      //hoek = 1;
      
      painter.save();
      painter.rotate(60*hoek);

      // teken alle bogen
      for (int kl = 0; kl<n_kleuren; kl++)
      {
         // de afstand
         int d = tegel->getEnd((kleur_t)kl) - tegel->getBegin((kleur_t)kl);
         // de hoek in aantal 60
         int h = tegel->getBegin((kleur_t)kl);
         
         if (d == 1)
         {
            roteer(painter, h);
            boog1(painter, (kleur_t)kl);
            painter.restore();
         }
         else
         if (d == 2)
         {
            roteer(painter, h);
            boog2(painter, (kleur_t)kl);
            painter.restore();
         }
         else
         if (d == 3)
         {
            roteer(painter, h - 1);
            boog3(painter, (kleur_t)kl);
            painter.restore();
         }
         else
         {
            throw BoogFout();
         }
      }
      painter.restore();
   }
}


/*
 * eerste versie

template<typename T>
struct Generator
{
   struct promise_type;
   using handle_type = std::coroutine_handle<promise_type>;

   struct promise_type
   {
      T value_;
      std::exception_ptr exception_;

      Generator get_return_object()
      {
         std::cout << "get_return_object()\n";
         return Generator(handle_type::from_promise(*this));
      }
      std::suspend_always initial_suspend()
      {
         std::cout << "intial_suspend()\n";
         return {};
      }
      std::suspend_always final_suspend() noexcept(true)
      {
         std::cout << "final_suspend()\n";
         return {};
      }
      void unhandled_exception()
      {
         std::cout << "unhandled_exception()\n";
         exception_ = std::current_exception();
      }
      template<std::convertible_to<T> From> // C++20 concept
      std::suspend_always yield_value(From &&from)
      {
         std::cout << "yield_value\n";
         value_ = std::forward<From>(from);
         return {};
      }
      void return_void()
      {
         std::cout << "return_void()\n";
      }
   };

   handle_type h_;

   Generator(handle_type h) : h_(h)
   {
      std::cout << "Generator()\n";
   }
   ~Generator()
   {
      std::cout << "~Generator()\n";
      h_.destroy();
   }
   explicit operator bool()
   {
      std::cout << "operator bool\n";
      fill();
      return !h_.done();
   }
   T operator()()
   {
      std::cout << "operator()\n";
      fill();
      full_ = false;
      return std::move(h_.promise().value_);
   }

private:
   bool full_ = false;

   void fill()
   {
      std::cout << "fill()\n";
      if (!full_)
      {
         std::cout << "   not full_\n";
         h_();
         if (h_.promise().exception_)
         {
            std::cout << "      exception\n";
            std::rethrow_exception(h_.promise().exception_);
         }
         full_ = true;
      }
      else
      {
         std::cout << "   full_\n";
      }
   }
};
 */

// tweede versie
template<typename T>
class Generator
{
public:
   struct promise_type;
   using promise_handle_t = std::coroutine_handle<promise_type>;
   explicit Generator(promise_handle_t h) : handle(h)
   {
   }
   explicit Generator(Generator &&generator) : handle(std::exchange(generator.handle, {}))
   {
   }
   ~Generator()
   {
      if (handle)
      {
         handle.destroy();
      }
   }
   Generator(Generator &) = delete;
   Generator&operator=(Generator &) = delete;
   
   struct promise_type
   {
      Generator get_return_object()
      {
         std::cout << "get_return_object\n";
         return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
      }

      void unhandled_exception() noexcept
      {
         std::cout << "unhandled_exception\n";
      }

      void return_void() noexcept
      {
         std::cout << "return_void\n";
      }

      std::suspend_never initial_suspend() noexcept
      {
         std::cout << "initial_suspend\n";
         return {};
      }
      std::suspend_never final_suspend() noexcept
      {
         std::cout << "final_suspend\n";
         return {};
      }
      std::suspend_always yield_value(T t)
      {
         v = std::move(t);
         return {};
      }
      
   //private:
      T v{};
   };

   bool has_next()
   {
      return !handle.done();
   }
   
   T next()
   {
      handle.resume();
      return handle.promise().v;
   }

   void resume()
   {
      handle.resume();
   }

   T value()
   {
      std::cout << "value " << handle.promise().v << "\n";
      return std::move(handle.promise().v);
   }

private:
   std::coroutine_handle<promise_type> handle;
};


class Bord
{
private:
   std::array<std::unique_ptr<Tegel>, n_tegels> tegels; // alle mogelijk tegels
   int                                          aantal; // effectief aantal tegels in dit spel
   std::array<std::array<std::shared_ptr<Plaats>, bordsize>, bordsize> plaatsen;
   kleur_t                                      ringkleur;
   std::shared_ptr<Plaats>                      eerste;
   std::shared_ptr<Plaats>                      laatste;
   
public:
   Bord(int n);
   Bord (const Bord &van);
   //Bord &operator=(const Bord &van);
   void toon();
   void zetburen();
   void zet_starttegel();
   int  tegels_op_bord();
   bool gelijke_kleuren();
   bool ring_niet_dood();
   void zet_ringkleur();
   bool einde();
   void teken(QPainter &painter);
   std::unique_ptr<Bord> solve(int d);
   Generator<std::unique_ptr<Bord>> solve_step(int d);
   std::unique_ptr<Bord> solve_co();
};


Bord::Bord(int n) : aantal(n), eerste(nullptr), laatste(nullptr),
   tegels
   {
      std::make_unique<Tegel>(G,  1, B, R, G, G, B, R),
      std::make_unique<Tegel>(G,  2, R, B, G, G, B, R),
      std::make_unique<Tegel>(G,  3, G, R, R, B, B, G),
      std::make_unique<Tegel>(R,  4, G, B, G, R, B, R),
      std::make_unique<Tegel>(R,  5, G, R, B, B, R, G),
      std::make_unique<Tegel>(B,  6, R, G, R, B, G, B),
      std::make_unique<Tegel>(R,  7, G, R, B, B, G, R),
      std::make_unique<Tegel>(B,  8, R, G, B, B, R, G),
      std::make_unique<Tegel>(G,  9, G, R, G, B, R, B),
      std::make_unique<Tegel>(B, 10, R, B, G, G, R, B)
   }
{
   //std::cout << "Bord::Bord\n";

   /*
   tegels.push_back(std::make_unique<Tegel>(G,  1, B, R, G, G, B, R));
   tegels.push_back(std::make_unique<Tegel>(G,  2, R, B, G, G, B, R));
   tegels.push_back(std::make_unique<Tegel>(G,  3, G, R, R, B, B, G));
   tegels.push_back(std::make_unique<Tegel>(R,  4, G, B, G, R, B, R));
   tegels.push_back(std::make_unique<Tegel>(R,  5, G, R, B, B, R, G));
   tegels.push_back(std::make_unique<Tegel>(B,  6, R, G, R, B, G, B));
   tegels.push_back(std::make_unique<Tegel>(R,  7, G, R, B, B, G, R));
   tegels.push_back(std::make_unique<Tegel>(B,  8, R, G, B, B, R, G));
   tegels.push_back(std::make_unique<Tegel>(G,  9, G, R, G, B, R, B));
   tegels.push_back(std::make_unique<Tegel>(B, 10, R, B, G, G, R, B));
    */

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         plaatsen[r][k] = std::make_shared<Plaats>();
         plaatsen[r][k]->set_rk(r, k);
      }
   }
   
   zetburen();
}

Bord::Bord(const Bord &van) : aantal(van.aantal), ringkleur(van.ringkleur), eerste(van.eerste), laatste(van.laatste)
{
   //std::cout << "Bord::Bord(Bord)\n";
   for (int i=0; i<n_tegels; i++)
   {
      //std::cout << "van.tegels[i] " << van.tegels[i] << "\n";
      if (van.tegels[i] != nullptr)
      {
         tegels[i] = std::make_unique<Tegel>(*van.tegels[i]);
      }
      else
      {
         tegels[i] = nullptr;
      }
   }

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         plaatsen[r][k] = std::make_shared<Plaats>(*van.plaatsen[r][k]);
      }
   }

   zetburen();
}   

void Bord::zet_ringkleur()
{
   ringkleur = tegels[aantal - 1]->get_kleur();
}

/*
Bord &Bord::operator=(const Bord &van)
{
   for (int i=0; i<n_tegels; i++)
   {
      tegels[i] = std::make_unique<Tegel>(*van.tegels[i]);
   }

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         plaatsen[r][k] = std::make_shared<Plaats>();
      }
   }
   
   return *this;
}   
 */
void Bord::toon()
{
   for (int i=0; i<n_tegels; i++)
   {
      if (tegels[i] != nullptr)
      {
         std::cout << "tegel niet nul\n";
      }
      else
      {
         std::cout << "tegel nul\n";
      }
   }

   for (int r=0; r<bordsize; r++)
   {
      if (r%2 == 0)
      {
         std::cout << " ";
      }
      for (int k=0; k<bordsize; k++)
      {
         plaatsen[r][k]->toon();
         std::cout << " ";
      }
      std::cout << "\n";
   }
}

// Verbind met de buren
//
// *-*-*
// |\|\|
// *-*-*
// |/|/|
// *-*-*
//
//  *-*-*
// /\/\/
// *-*-*
// \/\/\
//  *-*-*


void Bord::zetburen()
{
   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         for (int ri=0; ri<n_richtingen; ri++)
         {
            plaatsen[r][k]->zet_buur((richting_t)ri, nullptr);
         }
      }
   }

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         // NO
         if (r%2 == 0)
         {
            if (r > 0 && k > 0)
            {
               plaatsen[r][k]->zet_buur(NO, plaatsen[r-1][k+1]);
            }
         }
         else
         {
            if (r > 0)
            {
               plaatsen[r][k]->zet_buur(NO, plaatsen[r-1][k]);
            }
         }

         // O
         if (k < bordsize-1)
         {
            plaatsen[r][k]->zet_buur(O, plaatsen[r][k+1]);
         }

         // ZO
         if (r%2 == 0)
         {
            if (r < bordsize-1 && k < bordsize-1)
            {
               plaatsen[r][k]->zet_buur(ZO, plaatsen[r+1][k+1]);
            }
         }
         else
         {
            if (r < bordsize-1)
            {
               plaatsen[r][k]->zet_buur(ZO, plaatsen[r+1][k]);
            }
         }

         // ZW
         if (r%2 == 0)
         {
            if (r < bordsize-1)
            {
               plaatsen[r][k]->zet_buur(ZW, plaatsen[r+1][k]);
            }
         }
         else
         {
            if (r < bordsize-1 && k > 0)
            {
               plaatsen[r][k]->zet_buur(ZW, plaatsen[r+1][k-1]);
            }
         }
         
         // W
         if (k > 0)
         {
            plaatsen[r][k]->zet_buur(W, plaatsen[r][k-1]);
         }
         
         // NW
         if (r%2 == 0)
         {
            if (r > 0)
            {
               plaatsen[r][k]->zet_buur(NW, plaatsen[r-1][k]);
            }
         }
         else
         {
            if (r > 0 && k > 0)
            {
               plaatsen[r][k]->zet_buur(NW, plaatsen[r-1][k-1]);
            }
         }
      }
      //std::cout << "\n";
   }
}

void Bord::zet_starttegel()
{
   plaatsen[startr][startk]->zet_tegel(std::move(tegels[0]));
   eerste = plaatsen[bordsize/2][bordsize/2];
}

int Bord::tegels_op_bord()
{
   int n = 0;
   for (int i=0; i<aantal; i++)
   {
      //std::cout << "tob " << i << " " << aantal << "\n";
      if (tegels[i] == nullptr)
      {
         n++;
      }
   }
   
   return n;
}

// test alle overgangen op gelijke kleur
bool Bord::gelijke_kleuren()
{
   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         if (plaatsen[r][k]->bezet())
         {
            //std::cout << "plaats niet nul " << r  << " " << k <<"\n";
               
            // overloop de buren aan de 6 zijden
            for (int ri = 0; ri<zijden; ri++)
            {
               std::shared_ptr<Plaats> buur = plaatsen[r][k]->get_buur((richting_t)ri);
               if (buur != nullptr)
               {
                  if (buur->bezet())
                  {
                     // er is een overgang naar een buur
                     auto [rbu, kbu] = buur->get_rk();
                     //std::cout << "   " << ri << " volle buur " << rbu << " " << kbu << "\n";
                  
                     // haal de kleur van deze plaats in richting ri
                     int kl = plaatsen[r][k]->get_kleur((richting_t)ri);
                     //std::cout << "   " << ri << " kleur " << kl << "\n";
                  
                     // haal de tegenoverliggende kleur bij de buur
                     int opri   = opposite(ri);
                     int buurkl = buur->get_kleur((richting_t)opri);
                  
                     // bij verschil
                     if (buurkl != kl)
                     {
                        return false;
                     }
                  }
               }
               /*
               else
               {
                  // er is geen buur aan deze zijde

                  // haal de kleur van deze plaats in richting ri
                  int kl = plaatsen[r][k]->get_kleur((richting_t)ri);
                  //std::cout << "test dood kl " << kl << "\n";
                  //std::cout << "test dood ringkleur " << ringkleur << "\n";
                  if (kl == ringkleur)
                  {
                     //std::cout << "ring loopt dood\n";
                     return false;
                  }
               }
                */
            }
         }
      }
   }
   return true;
}

// test ring aaneensluitend
bool Bord::ring_niet_dood()
{
   //std::cout << "ring niet dood\n";

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         if (plaatsen[r][k]->bezet())
         {
            //std::cout << "   dood plaats niet nul " << r  << " " << k <<"\n";
               
            // overloop de buren aan de 6 zijden
            for (int ri = 0; ri<zijden; ri++)
            {
               std::shared_ptr<Plaats> buur = plaatsen[r][k]->get_buur((richting_t)ri);
               if (buur != nullptr)
               {
                  // er is een buur aan deze zijde

                  if (!buur->bezet())
                  {
                     // deze plaats is leeg
                     
                     // haal de kleur van deze plaats in richting ri
                     int kl = plaatsen[r][k]->get_kleur((richting_t)ri);
                     //std::cout << "      test dood kl " << kl << "\n";
                     //std::cout << "      test dood ringkleur " << ringkleur << "\n";
                     if (kl == ringkleur)
                     {
                        std::cout << "         ring loopt dood\n";
                        return false;
                     }
                  }
               }
               else
               {
                  // deze plaats zit aan de rand
                  return false;
               }
            }
         }
      }
   }
   return true;
}

bool Bord::einde()
{
   //std::cout << "tegels_op_bord " << tegels_op_bord() << "\n";
   //std::cout << "eerste aantal buren " << eerste->aantal_buren() << "\n";
   //return (tegels_op_bord() == aantal) && (eerste->aantal_buren() >= 2);
   return (tegels_op_bord() == aantal && gelijke_kleuren() && ring_niet_dood());
}

std::unique_ptr<Bord> Bord::solve(int d)
{
   //std::cout << "Bord::solve() " << d << "\n";
   
   if (einde())
   {
      std::cout << "einde1\n";
      return std::make_unique<Bord>(*this);
   }
   else
   {
      for (int r=0; r<bordsize; r++)
      {
         for (int k=0; k<bordsize; k++)
         {
            if (plaatsen[r][k]->bezet())
            {
               //std::cout << "plaats niet nul " << r  << " " << k <<"\n";
               
               // overloop de buren aan de 6 zijden
               for (int ri = 0; ri<zijden; ri++)
               {
                  std::shared_ptr<Plaats> buur = plaatsen[r][k]->get_buur((richting_t)ri);
                  if (buur != nullptr && !buur->bezet())
                  {
                     auto [rbu, kbu] = buur->get_rk();
                     //std::cout << "   " << ri << " lege buur " << rbu << " " << kbu << "\n";
                     int kl = plaatsen[r][k]->get_kleur((richting_t)ri);
                     //std::cout << "   " << ri << " kleur " << kl << "\n";
                     if (kl == ringkleur)
                     {
                        //std::cout << "      is ringkleur " << r << " " << k << "\n";
                        
                        // deze zijde is geschikt
                        // probeer elk van de overblijvende tegels te plaatsen
                        // bij buur
                        for (int ti=0; ti<aantal; ti++)
                        {
                           // is de tegel beschikbaar?
                           if (tegels[ti] != nullptr)
                           {
                              // plaats de tegel in het bord
                              buur->zet_tegel(std::move(tegels[ti]));
                              laatste = buur;
                              
                              // overloop alle hoeken
                              for (int ho=0; ho<zijden; ho++)
                              {
                                 buur->zet_hoek(ho);
                                 
                                 int opri   = opposite(ri);
                                 int buurkl = buur->get_kleur((richting_t)opri);
                                 
                                 // past de tegel?
                                 if (buurkl == ringkleur)
                                 {
                                    //std::cout << "         kleur past\n";
                                    std::unique_ptr<Bord> bord2 = std::make_unique<Bord>(*this);
                                    std::unique_ptr<Bord> bord3 = bord2->solve(d + 1);
                                    
                                    // zijn alle tegels geplaatst?
                                    //if (bord3->tegels_op_bord() == aantal)
                                    if (bord3->einde())
                                    {
                                       std::cout << "einde2\n";
                                       return std::make_unique<Bord>(*bord3);
                                    }
                                 }
                              }
                              // plaats de tegel terug in de reserve
                              tegels[ti] = buur->haalop_tegel();
                           }
                        }
                     }
                  }
                  else
                  {
                     //std::cout << "   " << ri << " volle buur\n";
                  }
               }
            }
         }
      }
      
      return std::make_unique<Bord>(*this);
   }
}



/*
Generator<unsigned> count()
{
   for (unsigned i = 0; i < 3;)
   {
      co_yield i++;
   }
}
 */

Generator<std::unique_ptr<Bord>> Bord::solve_step(int d)
{
   //std::cout << "Bord::solve() " << d << "\n";
   
   if (einde())
   {
      std::cout << "einde1\n";
      //return std::make_unique<Bord>(*this);
      co_yield std::make_unique<Bord>(*this);
   }
   else
   {
      for (int r=0; r<bordsize; r++)
      {
         for (int k=0; k<bordsize; k++)
         {
            if (plaatsen[r][k]->bezet())
            {
               //std::cout << "plaats niet nul " << r  << " " << k <<"\n";
               
               // overloop de buren aan de 6 zijden
               for (int ri = 0; ri<zijden; ri++)
               {
                  std::shared_ptr<Plaats> buur = plaatsen[r][k]->get_buur((richting_t)ri);
                  if (buur != nullptr && !buur->bezet())
                  {
                     auto [rbu, kbu] = buur->get_rk();
                     //std::cout << "   " << ri << " lege buur " << rbu << " " << kbu << "\n";
                     int kl = plaatsen[r][k]->get_kleur((richting_t)ri);
                     //std::cout << "   " << ri << " kleur " << kl << "\n";
                     if (kl == ringkleur)
                     {
                        //std::cout << "      is ringkleur " << r << " " << k << "\n";
                        
                        // deze zijde is geschikt
                        // probeer elk van de overblijvende tegels te plaatsen
                        // bij buur
                        for (int ti=0; ti<aantal; ti++)
                        {
                           // is de tegel beschikbaar?
                           if (tegels[ti] != nullptr)
                           {
                              // plaats de tegel in het bord
                              buur->zet_tegel(std::move(tegels[ti]));
                              laatste = buur;
                              
                              // overloop alle hoeken
                              for (int ho=0; ho<zijden; ho++)
                              {
                                 buur->zet_hoek(ho);
                                 
                                 int opri   = opposite(ri);
                                 int buurkl = buur->get_kleur((richting_t)opri);
                                 
                                 // past de tegel?
                                 if (buurkl == ringkleur)
                                 {
                                    //std::cout << "         kleur past\n";
                                    const std::unique_ptr<Bord> bord2 = std::make_unique<Bord>(*this);
                                    std::unique_ptr<Bord> bord3 = std::move(bord2->solve_step(d + 1).value());
                                    
                                    // zijn alle tegels geplaatst?
                                    //if (bord3->tegels_op_bord() == aantal)
                                    if (bord3->einde())
                                    {
                                       std::cout << "einde2\n";
                                       //return std::make_unique<Bord>(*bord3);
                                       co_yield std::make_unique<Bord>(*bord3);
                                    }
                                 }
                              }
                              // plaats de tegel terug in de reserve
                              tegels[ti] = buur->haalop_tegel();
                           }
                        }
                     }
                  }
                  else
                  {
                     //std::cout << "   " << ri << " volle buur\n";
                  }
               }
            }
         }
      }
      
      //return std::make_unique<Bord>(*this);
      co_yield std::make_unique<Bord>(*this);
   }
}

std::unique_ptr<Bord> Bord::solve_co()
{
   auto gen = solve_step(0);
   while (!gen.has_next())
   {
      std::cout << "counter6: " << gen.value() << std::endl;
      gen.resume();
   }
   return gen.value();
}




void Bord::teken(QPainter &painter)
{
   //std::cout << "Bord::teken()\n";

   for (int r=0; r<bordsize; r++)
   {
      for (int k=0; k<bordsize; k++)
      {
         int dx = 0;
         if (r%2 == 0)
         {
            dx = tegel_br/2;
         }
         painter.save();
         painter.translate(boord + k*tegel_br + tegel_br/2 + dx, boord + r*1.5*straal + tegel_br/2);
         
         // testlijn vanuit oorsprong naar rechts
         // voor de test
         //painter.drawLine( 0, 0, tegel_br/2, 0);
         
         //std::cout << "teken r k " << r << " " << k << "\n";
         if (plaatsen[r][k] != nullptr)
         {
            plaatsen[r][k]->teken(painter);
         }

         painter.restore();
      }
   }
}


int main2(int argc, char *args[])
{
   Bord bord(3);
   bord.toon();
   bord.zetburen();

   return 0;
}

#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QTime>
#include <QTimer>

//-------------- Klok -------------------

class Klok : public QWidget
{
   Q_OBJECT
   
public:
   Klok(QWidget *parent = nullptr);

protected:
   void paintEvent(QPaintEvent *event) override;   
};

class Venster : public QWidget
{
   Q_OBJECT
private:
   std::unique_ptr<Bord> bord;   

public:
   Venster(std::unique_ptr<Bord> brd, QWidget *parent = nullptr);

protected:
   void paintEvent(QPaintEvent *event) override;   
};

#include "tantrix.moc"


Klok::Klok(QWidget *parent)
    : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Klok::update));
    timer->start(1000);

    setWindowTitle(tr("Klok"));
    resize(200, 200);
}

void Klok::paintEvent(QPaintEvent *)
{
   static const QPoint hourHand[4] = 
   {
      QPoint(5, 14),
      QPoint(-5, 14),
      QPoint(-4, -71),
      QPoint(4, -71)
   };
   static const QPoint minuteHand[4] = 
   {
      QPoint(4, 14),
      QPoint(-4, 14),
      QPoint(-3, -89),
      QPoint(3, -89)
   };

   static const QPoint secondsHand[4] = 
   {
      QPoint(1, 14),
      QPoint(-1, 14),
      QPoint(-1, -89),
      QPoint(1, -89)
   };

   const QColor hourColor(palette().color(QPalette::Text));
   const QColor minuteColor(palette().color(QPalette::Text));
   const QColor secondsColor(palette().color(QPalette::Text));

   int side = qMin(width(), height());
   
   QPainter painter(this);
   QTime time = QTime::currentTime();

   painter.setRenderHint(QPainter::Antialiasing);
   
   painter.translate(width() / 2, height() / 2);
   painter.scale(side / 200.0, side / 200.0);
   
   painter.setPen(Qt::NoPen);
   painter.setBrush(hourColor);
    
   painter.save();
   painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
   painter.drawConvexPolygon(hourHand, 4);
   painter.restore();
    
    
   for (int i = 0; i < 12; ++i)
   {
      painter.drawRect(73, -3, 16, 6);
      painter.rotate(30.0);
   }
    
   painter.setBrush(minuteColor);

   painter.save();
   painter.rotate(6.0 * time.minute());
   painter.drawConvexPolygon(minuteHand, 4);
   painter.restore();

   painter.setBrush(secondsColor);

   painter.save();
   painter.rotate(6.0 * time.second());
   painter.drawConvexPolygon(secondsHand, 4);
   painter.drawEllipse(-3, -3, 6, 6);
   painter.drawEllipse(-5, -68, 10, 10);
   painter.restore();

   painter.setPen(minuteColor);

   for (int j = 0; j < 60; ++j) 
   {
      painter.drawLine(92, 0, 96, 0);
      painter.rotate(6.0);
   }
}



Venster::Venster(std::unique_ptr<Bord> brd, QWidget *parent)
    : bord(std::move(brd)), QWidget(parent)
{
   // niet nodig
   //QTimer *timer = new QTimer(this);
   //connect(timer, &QTimer::timeout, this, QOverload<>::of(&Venster::update));
   //timer->start(1000);

   constexpr int sz = 800;
   setWindowTitle(tr("Venster"));
   resize(sz, sz);
}

void Venster::paintEvent(QPaintEvent *)
{
   const int side     = qMin(width(), height()); // zijde in pixels
   //std::cout << "veld_br " << veld_br << "\n";
   QPainter painter(this);

   painter.setRenderHint(QPainter::Antialiasing);
   
   //painter.translate(width() / 2, height() / 2);
   //painter.translate(boord, boord);

   const double sc = side / ((double)veld_br);
   painter.scale(sc, sc);

   /*
   QPen pen;
   pen.setWidth(3);
   pen.setColor(Qt::red);
   //painter.setPen(QPen(Qt::red, O));
   painter.setPen(pen);

   // test diagonaal
   painter.drawLine(boord, boord, veld_br-boord, veld_br-boord);
    */

   //pen.setColor(Qt::green);
   //painter.setPen(pen);
   
   //std::cout << "straal " << straal << "\n";

   bord->teken(painter);
}



// ------------- main -----------------

int main3(int argc, char **argv)
{
   QApplication app (argc, argv);

   QPushButton button ("Hello world !");
   button.show();

   return app.exec();
}


int main4(int argc, char **argv)
{
   QApplication app (argc, argv);

   QWidget window;
   window.setFixedSize(300, 200);

   QPushButton *button = new QPushButton("Hello World", &window);
   button->setGeometry(10, 10, 80, 30);

   main2(argc, argv);
 
   window.show();

   return app.exec();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::cout << "maak bord\n";
    std::unique_ptr<Bord> bord = std::make_unique<Bord>(4);
    //Bord bord(3);
    bord->zet_starttegel();
    bord->zet_ringkleur();
    std::unique_ptr<Bord> res_bord = bord->solve(0);
    
    std::unique_ptr<Bord> bord2 = std::make_unique<Bord>(4);
    std::unique_ptr<Bord> res_bord2 = bord2->solve_co();
    //res_bord->toon();

    /*
    std::unique_ptr<Bord> bord2;
    std::cout << "dit is bord2\n";
    
    // voor de test
    std::cout << "kopieer bord naar bord2\n";
    bord2 = std::make_unique<Bord>(*bord);
     */
    // voor de test
    //Bord bord3(3);
    //Bord bord4(bord3);

    // voor de test van de Tegel copy constructor
    //std::unique_ptr<Tegel> teg = std::make_unique<Tegel>(G,  1, B, R, G, G, B, R);
    //std::unique_ptr<Tegel> teg2;
    //teg2 = std::make_unique<Tegel>(*teg);
    
    Venster venster(std::move(res_bord2));
    venster.show();
    return app.exec();
}

