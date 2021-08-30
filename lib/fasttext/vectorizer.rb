module FastText
  class Vectorizer < Model
    DEFAULT_OPTIONS = {
      lr: 0.5,
      lr_update_rate: 100,
      dim: 100,
      ws: 5,
      epoch: 5,
      min_count: 1,
      min_count_label: 0,
      neg: 5,
      word_ngrams: 1,
      loss: "ns",
      model: "skipgram",
      bucket: 2000000,
      minn: 3,
      maxn: 6,
      thread: 3,
      t: 0.0001,
      verbose: 2,
      pretrained_vectors: "",
      save_output: false,
      seed: 0,
      autotune_validation_file: "",
      autotune_metric: "f1",
      autotune_predictions: 1,
      autotune_duration: 60 * 5,
      autotune_model_size: ""
    }

    def fit(x)
      @m ||= Ext::Model.new
      a = build_args(DEFAULT_OPTIONS)
      a.input = input_path(x)
      m.train(a)
    end

    def nearest_neighbors(word, k: 10)
      m.nearest_neighbors(word, k).map(&:reverse).to_h
    end

    def analogies(word_a, word_b, word_c, k: 10)
      m.analogies(k, word_a, word_b, word_c).map(&:reverse).to_h
    end

    private

    # separate example by newlines
    # https://github.com/facebookresearch/fastText/issues/518
    def input_path(x)
      if x.is_a?(String)
        x
      else
        tempfile = Tempfile.new("fasttext")
        x.each do |xi|
          tempfile.write(xi.gsub("\n", " ")) # replace newlines in document
          tempfile.write("\n")
        end
        tempfile.close
        tempfile.path
      end
    end
  end
end
