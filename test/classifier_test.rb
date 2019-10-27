require_relative "test_helper"

class ClassifierTest < Minitest::Test
  def test_works
    x = [
      "This is the first document",
      "Hello, this is the second document",
      "Hello, and this is the third one",
      "Is this the first document?"
    ]
    y = ["ham", "spam", "spam", "ham"]

    model = FastText::Classifier.new
    model.fit(x, y)

    assert_equal 100, model.dimension
    assert !model.quantized?
    assert_equal 14, model.words.size
    assert_equal model.words, model.words(include_freq: true).keys
    assert_equal ["ham", "spam"].sort, model.labels.sort
    assert_equal model.labels, model.labels(include_freq: true).keys

    assert model.word_id("first")
    assert model.subwords("first")
    assert model.word_vector("first")
    assert model.sentence_vector("first document")

    assert model.predict("First document")
    assert model.predict(["First document", "Second document"], k: 3)

    # TODO fix flaky test
    # pred = model.predict("First document").first
    # assert_equal "ham", pred[0]
    # assert_in_delta 0.50003284, pred[1]

    result = model.test(x, y)
    assert_equal 4, result[:examples]
    assert_equal 1.0, result[:precision]
    assert_equal 1.0, result[:recall]

    model_path = "#{Dir.mktmpdir}/model.bin"
    model.save_model(model_path)
    model = FastText.load_model(model_path)

    assert_equal 100, model.dimension
    assert !model.quantized?
    assert_equal 14, model.words.size

    # takes a while
    # model.quantize
    # model.save_model("#{Dir.mktmpdir}/model.ftz")
    # assert model.quantized?
  end

  def test_input_file
    model = FastText::Classifier.new
    model.fit("test/support/supervised.txt")

    assert_equal 100, model.dimension
    assert !model.quantized?
    assert_equal 14, model.words.size
    assert_equal model.words, model.words(include_freq: true).keys
    assert_equal ["ham", "spam"].sort, model.labels.sort
    assert_equal model.labels, model.labels(include_freq: true).keys
  end

  def test_train_supervised
    model = FastText.train_supervised(input: "test/support/supervised.txt")

    assert_equal 100, model.dimension
    assert !model.quantized?
    assert_equal 14, model.words.size
    assert_equal model.words, model.words(include_freq: true).keys
    assert_equal ["ham", "spam"].sort, model.labels.sort
    assert_equal model.labels, model.labels(include_freq: true).keys
  end

  def test_untrained
    model = FastText::Classifier.new
    error = assert_raises FastText::Error do
      model.dimension
    end
    assert_equal "Not fit", error.message
  end

  def test_language_identification
    skip # don't want to include lid.176 with project

    model = FastText.load_model("path/to/lid.176.ftz")
    p model.predict("bon appétit")
  end
end
