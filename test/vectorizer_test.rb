require_relative "test_helper"

class VectorizerTest < Minitest::Test
  def test_works
    x = [
      "this is a test"
    ]

    model = FastText::Vectorizer.new
    model.fit(x)

    assert model.nearest_neighbors("asparagus")
    assert model.analogies("berlin", "germany", "france")
  end

  def test_input_file
    model = FastText::Vectorizer.new
    model.fit("test/support/unsupervised.txt")

    assert model.nearest_neighbors("asparagus")
    assert model.analogies("berlin", "germany", "france")
  end

  def test_train_unsupervised
    model = FastText.train_unsupervised(input: "test/support/unsupervised.txt")

    assert model.nearest_neighbors("asparagus")
    assert model.analogies("berlin", "germany", "france")
  end
end
