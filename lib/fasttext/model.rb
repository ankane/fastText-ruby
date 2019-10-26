module FastText
  class Model
    def initialize(**options)
      @options = options
    end

    def dimension
      m.dimension
    end

    def quantized?
      m.quantized?
    end

    def save_model(path)
      m.save_model(path)
    end

    def words(include_freq: false)
      words, freqs = m.words
      if include_freq
        words.zip(freqs).to_h
      else
        words
      end
    end

    def word_vector(word)
      m.word_vector(word)
    end

    def sentence_vector(text)
      m.sentence_vector(prep_text(text))
    end

    def word_id(word)
      m.word_id(word)
    end

    def subword_id(subword)
      m.subword_id(subword)
    end

    def subwords(word)
      m.subwords(word)
    end

    private

    # text must end in newline for prediction to match Python and CLI
    def prep_text(text)
      text = "#{text}\n" unless text.end_with?("\n")
      text
    end

    def m
      @m || (raise Error, "Not fit")
    end
  end
end
