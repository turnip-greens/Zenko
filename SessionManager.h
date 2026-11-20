#pragma once
#include "CardEngine.h"

namespace Zenko {

    using namespace System;
    using namespace System::Collections::Generic;

    // ── Question Types ────────────────────────────────────
    // What the player is being asked each round.
    public enum class QuestionType {
        KanjiToMeaning,     // show kanji, pick meaning
        MeaningToKanji,     // show meaning, pick kanji
        KanjiToReading,     // show kanji, pick onyomi
        ReadingToKanji,     // show onyomi, pick kanji (only if reading is unique)
    };

    // ── Question ──────────────────────────────────────────
    // Everything the UI needs to display one question.
    public ref struct Question {
        Card^ card;           // the card being tested
        QuestionType    type;           // what kind of question it is
        String^ prompt;         // the instruction label e.g. "What does this mean?"
        String^ questionText;   // the big text shown to the player
        String^ correctAnswer;  // what counts as correct
        List<String^>^ choices;        // all 4 choices, already shuffled
    };

    // ── SessionManager ────────────────────────────────────
    // Owns one study session. Takes a list of cards from
    // CardEngine and handles everything that happens during
    // study: picking cards, building questions, tracking score.
    public ref class SessionManager {
    public:
        SessionManager(List<Card^>^ sessionCards, CardEngine^ engine);

        // Get the next question — call this to advance the session
        Question^ NextQuestion();

        // Call this when the player answers
        // Returns true if correct
        bool        Answer(String^ given, Grade% gradeOut);

        // True when all due cards have been answered
        bool        IsComplete();

        // Score tracking
        property int Streak { int get() { return streak; } }
        property int TotalCorrect { int get() { return totalCorrect; } }
        property int TotalAnswered { int get() { return totalAnswered; } }
        property int CardCount { int get() { return sessionCards->Count; } }

    private:
        List<Card^>^ sessionCards;   // cards in this session
        CardEngine^ engine;         // reference back for UpdateProgress
        System::Random^ rng;

        // Current state
        Question^ currentQuestion;
        int             streak;
        int             totalCorrect;
        int             totalAnswered;

        // Builds a Question^ for the given card
        Question^ BuildQuestion(Card^ card);

        // Picks a valid question type for this card.
        // Avoids ReadingToKanji if the reading is ambiguous.
        QuestionType    PickQuestionType(Card^ card);

        // Returns 3 wrong answers of the right type
        List<String^>^ GetWrongAnswers(Card^ card, QuestionType type, String^ correctAnswer);

        // True if this card's onyomi appears on any other card in the session
        bool            IsReadingAmbiguous(Card^ card);

        // Shuffles a list in place
        void            Shuffle(List<String^>^ list);
    };
} 