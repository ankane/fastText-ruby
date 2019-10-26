# ext
require "fasttext/ext"

# modules
require "fasttext/model"
require "fasttext/classifier"
require "fasttext/vectorizer"
require "fasttext/version"

module FastText
  class Error < StandardError; end

  class << self
    def load_model(path)
      m = Ext::Model.new
      m.load_model(path)
      model =
        if m.supervised?
          FastText::Classifier.new
        else
          FastText::Vectorizer.new
        end
      model.instance_variable_set("@m", m)
      model
    end

    def train_supervised(**options)
      input = options.delete(:input)
      model = FastText::Classifier.new(**options)
      model.fit(input)
      model
    end

    def train_unsupervised(**options)
      input = options.delete(:input)
      model = FastText::Vectorizer.new(**options)
      model.fit(input)
      model
    end
  end
end
