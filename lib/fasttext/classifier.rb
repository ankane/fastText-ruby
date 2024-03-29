module FastText
  class Classifier < Model
    DEFAULT_OPTIONS = {
      lr: 0.1,
      lr_update_rate: 100,
      dim: 100,
      ws: 5,
      epoch: 5,
      min_count: 1,
      min_count_label: 0,
      neg: 5,
      word_ngrams: 1,
      loss: "softmax",
      model: "supervised",
      bucket: 2000000,
      minn: 0,
      maxn: 0,
      thread: 3,
      t: 0.0001,
      label_prefix: "__label__",
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

    def fit(x, y = nil, autotune_set: nil)
      input, _ref = input_path(x, y)
      @m ||= Ext::Model.new
      a = build_args(DEFAULT_OPTIONS)
      a.input = input
      a.model = "supervised"
      if autotune_set
        x, y = autotune_set
        a.autotune_validation_file, _autotune_ref = input_path(x, y)
      end
      m.train(a)
    end

    def predict(text, k: 1, threshold: 0.0)
      multiple = text.is_a?(Array)
      text = [text] unless multiple

      # TODO predict multiple in C++ for performance
      result =
        text.map do |t|
          m.predict(prep_text(t), k, threshold).to_h do |v|
            [remove_prefix(v[1]), v[0]]
          end
        end

      multiple ? result : result.first
    end

    def test(x, y = nil, k: 1)
      input, _ref = input_path(x, y)
      res = m.test(input, k)
      {
        examples: res[0],
        precision: res[1],
        recall: res[2]
      }
    end

    # TODO support options
    def quantize
      a = Ext::Args.new
      m.quantize(a)
    end

    def labels(include_freq: false)
      labels, freqs = m.labels
      labels.map! { |v| remove_prefix(v) }
      if include_freq
        labels.zip(freqs).to_h
      else
        labels
      end
    end

    private

    def input_path(x, y)
      if x.is_a?(String)
        raise ArgumentError, "Cannot pass y with file" if y
        [x, nil]
      else
        tempfile = Tempfile.new("fasttext")
        x.zip(y) do |xi, yi|
          parts = Array(yi).map { |label| "__label__" + label }
          parts << xi.gsub("\n", " ") # replace newlines in document
          tempfile.write(parts.join(" "))
          tempfile.write("\n")
        end
        tempfile.close
        [tempfile.path, tempfile]
      end
    end

    def remove_prefix(label)
      label.sub(label_prefix, "")
    end

    def label_prefix
      m.label_prefix
    end
  end
end
