#include "SessionManager.h"

namespace Zenko {

    // ── Constructor ───────────────────────────────────────
    // Takes the cards for this session and a reference to the
    // engine so we can call UpdateProgress after each answer.
    SessionManager::SessionManager(List<Card^>^ sessionCards, CardEngine^ engine) {
        this->sessionCards = sessionCards;
        this->engine = engine;
        this->rng = gcnew System::Random();
        this->streak = 0;
        this->totalCorrect = 0;
        this->totalAnswered = 0;
        this->currentQuestion = nullptr;
    }

    // ── IsComplete ────────────────────────────────────────
    // Session is done when the player has answered every card.
    bool SessionManager::IsComplete() {
        return totalAnswered >= sessionCards->Count;
    }

    // ── NextQuestion ──────────────────────────────────────
    // Picks a random card from the session, builds a question
    // for it, stores it as currentQuestion and returns it.
    // The UI calls this to get everything it needs to display.
    Question^ SessionManager::NextQuestion() {
        if (IsComplete()) return nullptr;

        // Pick a random card from the session pool
        int idx = rng->Next(sessionCards->Count);
        Card^ card = sessionCards[idx];

        currentQuestion = BuildQuestion(card);
        return currentQuestion;
    }

    // ── Answer ────────────────────────────────────────────
    // Call this with whatever the player picked.
    // Returns true if correct, and sets gradeOut so the caller
    // can pass it to CardEngine::UpdateProgress if they want
    // to override the default grading (e.g. for timed mode).
    bool SessionManager::Answer(String^ given, Grade% gradeOut) {
        if (currentQuestion == nullptr) {
            gradeOut = Grade::Wrong;
            return false;
        }

        bool correct = String::Compare(
            given->Trim(),
            currentQuestion->correctAnswer->Trim(),
            StringComparison::OrdinalIgnoreCase) == 0;

        totalAnswered++;

        if (correct) {
            streak++;
            totalCorrect++;
            gradeOut = Grade::Correct;  // UI can upgrade to Easy if it wants
        }
        else {
            streak = 0;
            gradeOut = Grade::Wrong;
        }

        // Tell the engine to update SRS state for this card
        engine->UpdateProgress(currentQuestion->card, gradeOut);

        return correct;
    }

    // ── BuildQuestion ─────────────────────────────────────
    // Creates a fully populated Question^ for the given card.
    // Picks the question type, gets the correct answer,
    // generates 3 wrong answers, shuffles all 4, done.
    Question^ SessionManager::BuildQuestion(Card^ card) {
        Question^ q = gcnew Question();
        q->card = card;
        q->type = PickQuestionType(card);
        q->choices = gcnew List<String^>();

        // Set prompt, question text, and correct answer
        // based on what type of question this is
        switch (q->type) {
        case QuestionType::KanjiToMeaning:
            q->prompt = "What does this kanji mean?";
            q->questionText = card->data->kanji;
            q->correctAnswer = card->data->meaning;
            break;

        case QuestionType::MeaningToKanji:
            q->prompt = "Which kanji matches this meaning?";
            q->questionText = card->data->meaning;
            q->correctAnswer = card->data->kanji;
            break;

        case QuestionType::KanjiToReading:
            q->prompt = "What is the reading of this kanji?";
            q->questionText = card->data->kanji;
            q->correctAnswer = card->data->onyomi;
            break;

        case QuestionType::ReadingToKanji:
            q->prompt = "Which kanji has this reading?";
            q->questionText = card->data->onyomi;
            q->correctAnswer = card->data->kanji;
            break;
        }

        // Build the 4 choices: 1 correct + 3 wrong, then shuffle
        q->choices->Add(q->correctAnswer);
        auto wrongs = GetWrongAnswers(card, q->type, q->correctAnswer);
        for each (String ^ w in wrongs) q->choices->Add(w);
        Shuffle(q->choices);

        return q;
    }

    // ── PickQuestionType ──────────────────────────────────
    // Randomly picks a question type valid for this card.
    // ReadingToKanji is excluded if the onyomi is ambiguous
    // (shared by another card) because then there'd be
    // multiple correct answers and the player can't win.
    // KanjiToReading and ReadingToKanji are also skipped
    // if the card has no onyomi data.
    QuestionType SessionManager::PickQuestionType(Card^ card) {
        auto valid = gcnew List<QuestionType>();

        // These two are always valid
        valid->Add(QuestionType::KanjiToMeaning);
        valid->Add(QuestionType::MeaningToKanji);

        // Reading questions only if the card has an onyomi
        bool hasReading = card->data->onyomi != "n/a" && card->data->onyomi->Length > 0;
        if (hasReading) {
            valid->Add(QuestionType::KanjiToReading);

            // ReadingToKanji only if no other card shares this reading
            if (!IsReadingAmbiguous(card))
                valid->Add(QuestionType::ReadingToKanji);
        }

        return valid[rng->Next(valid->Count)];
    }

    // ── IsReadingAmbiguous ────────────────────────────────
    // Returns true if any OTHER card in the session has the
    // same onyomi as this card. If so, ReadingToKanji would
    // have multiple correct answers, so we skip it.
    bool SessionManager::IsReadingAmbiguous(Card^ card) {
        for each (Card ^ other in sessionCards) {
            if (other == card) continue;
            if (other->data->onyomi == card->data->onyomi)
                return true;
        }
        return false;
    }

    // ── GetWrongAnswers ───────────────────────────────────
    // Picks 3 wrong answers of the correct field type.
    // Filters out: the correct answer, "n/a", duplicates,
    // and the current card itself.
    List<String^>^ SessionManager::GetWrongAnswers(Card^ card, QuestionType type, String^ correctAnswer) {
        auto wrongs = gcnew List<String^>();
        auto used = gcnew List<int>();
        auto allCards = engine->GetAllCards();
        int attempts = 0;

        while (wrongs->Count < 3 && attempts < 300) {
            attempts++;
            int idx = rng->Next(allCards->Count);
            if (used->Contains(idx)) continue;
            used->Add(idx);

            Card^ other = allCards[idx];
            if (other == card) continue;

            // Pick the field that matches the question type
            String^ wrong;
            switch (type) {
            case QuestionType::KanjiToMeaning:
                wrong = other->data->meaning; break;
            case QuestionType::MeaningToKanji:
            case QuestionType::ReadingToKanji:
                wrong = other->data->kanji; break;
            case QuestionType::KanjiToReading:
                wrong = other->data->onyomi; break;
            default:
                wrong = other->data->meaning; break;
            }

            if (wrong == correctAnswer) continue;
            if (wrong == "n/a")         continue;
            if (wrongs->Contains(wrong)) continue;

            wrongs->Add(wrong);
        }

        return wrongs;
    }

    // ── Shuffle ───────────────────────────────────────────
    // Fisher-Yates shuffle — randomly reorders the list in place.
    // This is how we make sure the correct answer isn't always
    // in the same button position.
    void SessionManager::Shuffle(List<String^>^ list) {
        for (int i = list->Count - 1; i > 0; i--) {
            int j = rng->Next(i + 1);
            String^ tmp = list[i];
            list[i] = list[j];
            list[j] = tmp;
        }
    }
} 