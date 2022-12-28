#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

Transaction::Transaction(std::string ticker_symbol, unsigned int day_date,
                         unsigned int month_date, unsigned year_date,
                         bool buy_sell_trans, unsigned int number_shares,
                         double trans_amount) : symbol{ticker_symbol},
                                                day{day_date},
                                                month{month_date},
                                                year{year_date},
                                                trans_type{buy_sell_trans},
                                                shares{number_shares},
                                                amount{trans_amount},
                                                trans_id{assigned_trans_id},
                                                p_next{nullptr},
                                                acb{0},
                                                acb_per_share{0},
                                                share_balance{0},
                                                cgl{0}
{
  if (buy_sell_trans)
  {
    trans_type = "Buy";
  }
  else
  {
    trans_type = "Sell";
  }
  ++assigned_trans_id;
}

// Destructor
// TASK 1
//
Transaction::~Transaction()
{
}

bool Transaction::operator<(Transaction const &other)
{
  if (get_year() < other.get_year())
  {
    return true;
  }
  else if ((get_year() == other.get_year()) && (get_month() < other.get_month()))
  {
    return true;
  }
  else if ((get_year() == other.get_year()) && (get_month() == other.get_month()) && (get_day() < other.get_day()))
  {
    return true;
  }
  else if ((get_trans_id() > other.get_trans_id()) && (get_day() == other.get_day()) && (get_month() == other.get_month()) && (get_year() == other.get_year()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true : false; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

void Transaction::set_acb(double acb_value) { acb = acb_value; }
void Transaction::set_acb_per_share(double acb_share_value) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance(unsigned int bal) { share_balance = bal; }
void Transaction::set_cgl(double value) { cgl = value; }
void Transaction::set_next(Transaction *p_new_next) { p_next = p_new_next; }

void Transaction::print()
{
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
            << std::setw(4) << get_symbol() << " "
            << std::setw(4) << get_day() << " "
            << std::setw(4) << get_month() << " "
            << std::setw(4) << get_year() << " ";

  if (get_trans_type())
  {
    std::cout << "  Buy  ";
  }
  else
  {
    std::cout << "  Sell ";
  }

  std::cout << std::setw(4) << get_shares() << " "
            << std::setw(10) << get_amount() << " "
            << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
            << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
            << std::setw(10) << std::setprecision(3) << get_cgl()
            << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//

History::History() : p_head{nullptr}
{
}

History::~History()
{
  while (p_head != nullptr)
  {
    Transaction *p_old{p_head};
    p_head = p_head->get_next();
    delete p_old;
    p_old = nullptr;
  }
}

void History::read_history()
{
  ece150::open_file();
  while (ece150::next_trans_entry())
  {
    History::insert(new Transaction{ece150::get_trans_symbol(), ece150::get_trans_day(), ece150::get_trans_month(), ece150::get_trans_year(), ece150::get_trans_type(), ece150::get_trans_shares(), ece150::get_trans_amount()});
  }
  ece150::close_file();
}

void History::insert(Transaction *p_new_trans)
{
  if (p_head != nullptr)
  {
    for (Transaction *p_current{p_head}; true; p_current = p_current->get_next())
    {
      if (p_current->get_next() == nullptr)
      {
        p_current->set_next(p_new_trans);
        break;
      }
    }
  }
  else
  {
    p_head = p_new_trans;
  }
}

void History::sort_by_date()
{
  Transaction *p_head_tmp{p_head};
  p_head = p_head->get_next();
  p_head_tmp->set_next(nullptr);
  for (Transaction *p_current{p_head}; p_current != nullptr; p_current = p_head)
  {
    p_head = p_head->get_next();
    for (Transaction *p_iterator{p_head_tmp}; p_iterator != nullptr; p_iterator = p_iterator->get_next())
    {
      // check if sorted list only has 1 entry
      if (p_head_tmp->get_next() == nullptr)
      {
        if (*p_current < *p_head_tmp)
        {
          p_current->set_next(p_head_tmp);
          p_head_tmp = p_current;
          break;
        }
        else
        {
          p_head_tmp->set_next(p_current);
          p_current->set_next(nullptr);
          break;
        }
      }
      // check if current transaction is less than the first entry of the sorted list
      else if (*p_current < *p_head_tmp)
      {
        p_current->set_next(p_head_tmp);
        p_head_tmp = p_current;
        break;
      }
      // check if we are on the 2nd last entry
      else if (p_iterator->get_next()->get_next() == nullptr)
      {
        if (*p_current < *(p_iterator->get_next()))
        {
          p_current->set_next(p_iterator->get_next());
          p_iterator->set_next(p_current);
          break;
        }
        else
        {
          p_iterator->get_next()->set_next(p_current);
          p_current->set_next(nullptr);
          break;
        }
      }
      // check if the current transaction comes before the next entry in the sorted list
      else if ((*p_current < *(p_iterator->get_next())))
      {
        p_current->set_next(p_iterator->get_next());
        p_iterator->set_next(p_current);
        break;
      }
    }
  }
  // set the original head of the linked list to the head for the sorted linked list
  p_head = p_head_tmp;
}

void History::update_acb_cgl()
{
  p_head->set_acb(p_head->get_amount());
  p_head->set_share_balance(p_head->get_shares());
  p_head->set_acb_per_share((p_head->get_acb()) / (p_head->get_share_balance()));
  p_head->set_cgl(0);
  for (Transaction *p_current{p_head}; p_current->get_next() != nullptr; p_current = p_current->get_next())
  {
    if (p_current->get_next()->get_trans_type())
    {
      p_current->get_next()->set_acb((p_current->get_acb()) + (p_current->get_next()->get_amount()));
      p_current->get_next()->set_share_balance(p_current->get_share_balance() + p_current->get_next()->get_shares());
      p_current->get_next()->set_acb_per_share((p_current->get_next()->get_acb()) / (p_current->get_next()->get_share_balance()));
      p_current->get_next()->set_cgl(0);
    }
    else
    {
      p_current->get_next()->set_acb((p_current->get_acb()) - ((p_current->get_next()->get_shares()) * (p_current->get_acb_per_share())));
      p_current->get_next()->set_share_balance((p_current->get_share_balance()) - (p_current->get_next()->get_shares()));
      p_current->get_next()->set_acb_per_share(p_current->get_acb_per_share());
      p_current->get_next()->set_cgl((p_current->get_next()->get_amount()) - ((p_current->get_next()->get_shares()) * (p_current->get_acb_per_share())));
    }
  }
}

double History::compute_cgl(unsigned int year)
{
  update_acb_cgl();
  double count{0};
  for (Transaction *p_iterator{p_head}; p_iterator != nullptr; p_iterator = p_iterator->get_next())
  {
    if (p_iterator->get_year() == year)
    {
      count += (p_iterator->get_cgl());
    }
  }
  return count;
}

void History::print()
{
  std::cout << "BEGIN TRANSACTION HISTORY" << std::endl;
  for (Transaction *p_current{p_head}; p_current != nullptr; p_current = p_current->get_next())
  {
    p_current->print();
  }
  std::cout << "END TRANSACTION HISTORY" << std::endl;
}

Transaction *History::get_p_head() { return p_head; }
