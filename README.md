# fastText

[fastText](https://fasttext.cc) - efficient text classification and representation learning - for Ruby

[![Build Status](https://travis-ci.org/ankane/fasttext.svg?branch=master)](https://travis-ci.org/ankane/fasttext)

## Installation

Add this line to your application’s Gemfile:

```ruby
gem 'fasttext'
```

## Getting Started

fastText has two primary use cases:

- [text classification](#text-classification)
- [word representations](#word-representations)

## Text Classification

Prep your data

```ruby
# documents
x = [
  "text from document one",
  "text from document two",
  "text from document three"
]

# labels
y = ["ham", "ham", "spam"]
```

> Use an array if a document has multiple labels

Train a model

```ruby
model = FastText::Classifier.new
model.fit(x, y)
```

Get predictions

```ruby
model.predict(x.first)
```

Save the model to a file

```ruby
model.save_model("model.bin")
```

Load the model from a file

```ruby
model = FastText.load_model("model.bin")
```

Evaluate the model

```ruby
model.test(x_test, y_test)
```

Get words and labels

```ruby
model.words
model.labels
```

> Use `include_freq: true` to get their frequency

Compress the model - significantly reduces size but sacrifices a little performance

```ruby
model.quantize
model.save_model("model.ftz")
```

## Word Representations

Prep your data

```ruby
x = [
  "text from document one",
  "text from document two",
  "text from document three"
]
```

Train a model

```ruby
model = FastText::Vectorizer.new
model.fit(x)
```

Get nearest neighbors

```ruby
model.nearest_neighbors("asparagus")
```

Get analogies

```ruby
model.analogies("berlin", "germany", "france")
```

Get a word vector

```ruby
model.word_vector("carrot")
```

Get words

```ruby
model.words
```

Save the model to a file

```ruby
model.save_model("model.bin")
```

Load the model from a file

```ruby
model = FastText.load_model("model.bin")
```

Use continuous bag-of-words

```ruby
model = FastText::Vectorizer.new(model: "cbow")
```

## Parameters

Text classification

```ruby
FastText::Classifier.new(
  lr: 0.1,                    # learning rate
  dim: 100,                   # size of word vectors
  ws: 5,                      # size of the context window
  epoch: 5,                   # number of epochs
  min_count: 1,               # minimal number of word occurences
  min_count_label: 1,         # minimal number of label occurences
  minn: 0,                    # min length of char ngram
  maxn: 0,                    # max length of char ngram
  neg: 5,                     # number of negatives sampled
  word_ngrams: 1,             # max length of word ngram
  loss: "softmax",            # loss function {ns, hs, softmax, ova}
  bucket: 2000000,            # number of buckets
  thread: 3,                  # number of threads
  lr_update_rate: 100,        # change the rate of updates for the learning rate
  t: 0.0001,                  # sampling threshold
  label_prefix: "__label__"   # label prefix
  verbose: 2,                 # verbose
  pretrained_vectors: nil     # pretrained word vectors (.vec file)
)
```

Word representations

```ruby
FastText::Vectorizer.new(
  model: "skipgram",          # unsupervised fasttext model {cbow, skipgram}
  lr: 0.05,                   # learning rate
  dim: 100,                   # size of word vectors
  ws: 5,                      # size of the context window
  epoch: 5,                   # number of epochs
  min_count: 5,               # minimal number of word occurences
  minn: 3,                    # min length of char ngram
  maxn: 6,                    # max length of char ngram
  neg: 5,                     # number of negatives sampled
  word_ngrams: 1,             # max length of word ngram
  loss: "ns",                 # loss function {ns, hs, softmax, ova}
  bucket: 2000000,            # number of buckets
  thread: 3,                  # number of threads
  lr_update_rate: 100,        # change the rate of updates for the learning rate
  t: 0.0001,                  # sampling threshold
  verbose: 2                  # verbose
)
```

## Input Files

Input can be read directly from files

```ruby
model.fit("train.txt")
model.test("test.txt")
```

Each line should be a document

```txt
text from document one
text from document two
text from document three
```

For text classification, lines should start with a list of labels prefixed with `__label__`

```txt
__label__ham text from document one
__label__ham text from document two
__label__spam text from document three
```

## Pretrained Models

There are a number of [pretrained models](https://fasttext.cc/docs/en/english-vectors.html) you can download

### Language Identification

Download one of the [pretrained models](https://fasttext.cc/docs/en/language-identification.html) and load it

```ruby
model = FastText.load_model("lid.176.ftz")
```

Get language predictions

```ruby
model.predict("bon appétit")
```

## rbenv

This library uses [Rice](https://github.com/jasonroelofs/rice) to interface with the fastText C++ library. Unfortunately, Rice and rbenv don’t play nicely together. This is actively [being addressed](https://github.com/rbenv/ruby-build/pull/1368), but in the meantime, if you encounter an error during installation, reinstall your Ruby version with the `--enable-shared` flag.

```sh
CONFIGURE_OPTS="--enable-shared" rbenv install 2.6.5
```

## History

View the [changelog](https://github.com/ankane/fasttext/blob/master/CHANGELOG.md)

## Contributing

Everyone is encouraged to help improve this project. Here are a few ways you can help:

- [Report bugs](https://github.com/ankane/fasttext/issues)
- Fix bugs and [submit pull requests](https://github.com/ankane/fasttext/pulls)
- Write, clarify, or fix documentation
- Suggest or add new features

To get started with development and testing:

```sh
git clone https://github.com/ankane/fasttext.git
cd fasttext
bundle install
rake compile
rake test
```
