#pragma once

#include <map>
#include <functional>
#include "./quote.h"

namespace hftbattle {

using QuotesType = std::map<Price, std::unique_ptr<Quote>,
  std::function<bool(const Price &, const Price &)>>;

class QuotesHolder {
public:
  using BaseConstIterator = QuotesType::const_iterator;

  class QuotesHolderIterator: public BaseConstIterator {
  public:
    using value_type = Quote;
    explicit QuotesHolderIterator(BaseConstIterator it): BaseConstIterator(it) {}

    const value_type* operator->() const {
      return &**this;
    }

    const value_type& operator*() const {
      return *this->BaseConstIterator::operator->()->second;
    }

  };

  using const_iterator = QuotesHolderIterator;
  using value_type = QuotesHolderIterator::value_type;
  explicit QuotesHolder(const QuotesType* quotes) : quotes_(quotes) {}

  const_iterator begin() const {
    return QuotesHolderIterator(quotes_->begin());
  }

  const_iterator end() const {
    return QuotesHolderIterator(quotes_->end());
  }

private:
  const QuotesType* quotes_;
};

}  // namespace hftbattle
