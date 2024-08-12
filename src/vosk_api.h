// Copyright 2020-2024 Alpha Cephei Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* This header contains the C API for Vosk speech recognition system */

#ifndef VOSK_API_H
#define VOSK_API_H

#ifdef __cplusplus
extern "C" {
#endif

/** Model stores all the data required for recognition
 *  it contains static data and can be shared across processing
 *  threads. */
typedef struct VoskModel VoskModel;


/** Recognizer object is the main object which processes data.
 *  Each recognizer usually runs in own thread and takes audio as input.
 *  Once audio is processed recognizer returns JSON object as a string
 *  which represent decoded information - words, confidences, times, n-best lists,
 *  speaker information and so on */
typedef struct VoskRecognizer VoskRecognizer;


/** Loads model data from the file and returns the model object
 *
 * @param model_path: the path of the model on the filesystem
 * @returns model object or NULL if problem occured */
VoskModel *vosk_model_new(const char *model_path);


/** Releases the model memory
 *
 *  The model object is reference-counted so if some recognizer
 *  depends on this model, model might still stay alive. When
 *  last recognizer is released, model will be released too. */
void vosk_model_free(VoskModel *model);


/** Creates the recognizer object
 *
 *  The recognizers process the speech and return text using shared model data
 *  @param model       VoskModel containing static data for recognizer. Model can be
 *                     shared across recognizers, even running in different threads.
 *  @param sample_rate The sample rate of the audio you going to feed into the recognizer.
 *                     Make sure this rate matches the audio content, it is a common
 *                     issue causing accuracy problems.
 *  @returns recognizer object or NULL if problem occured */
VoskRecognizer *vosk_recognizer_new(VoskModel *model, float sample_rate);


/** Accept voice data
 *
 *  accept and process new chunk of voice data
 *
 *  @param data - audio data in PCM 16-bit mono format
 *  @param length - length of the audio data */
void vosk_recognizer_accept_waveform(VoskRecognizer *recognizer, const char *data, int length);


/** Same as above but the version with the short data for language bindings where you have
 *  audio as array of shorts */
void vosk_recognizer_accept_waveform_s(VoskRecognizer *recognizer, const short *data, int length);


/** Same as above but the version with the float data for language bindings where you have
 *  audio as array of floats */
void vosk_recognizer_accept_waveform_f(VoskRecognizer *recognizer, const float *data, int length);


/** Returns speech recognition result
 *
 * @returns the result in JSON format which contains decoded line, decoded
 *          words, times in seconds and confidences. You can parse this result
 *          with any json parser
 *
 * <pre>
 *  {
 *    "text" : "what zero zero zero one"
 *  }
 * </pre>
 *
 * If alternatives enabled it returns result with alternatives, see also vosk_recognizer_set_max_alternatives().
 *
 * If word times enabled returns word time, see also vosk_recognizer_set_word_times().
 */
const char *vosk_recognizer_result_front(VoskRecognizer *recognizer);


/** TODO: WRITEME */
void vosk_recognizer_result_pop(VoskRecognizer *recognizer);


/** Get number of pending chunks for more intelligent waiting */
int vosk_recognizer_get_num_pending_results(VoskRecognizer *recognizer);


/** Get number of already processed with results */
int vosk_recognizer_get_num_results(VoskRecognizer *recognizer);


/** If result queue is empty */
int vosk_recognizer_results_empty(VoskRecognizer *recognizer);


/** Flush all internal buffers and produce final results */
void vosk_recognizer_flush(VoskRecognizer *recognizer);


/** Resets the recognizer
 *
 *  Resets current results so the recognition can continue from scratch */
void vosk_recognizer_reset(VoskRecognizer *recognizer);

typedef enum VoskEpMode {
    VOSK_EP_ANSWER_DEFAULT = 0,
    VOSK_EP_ANSWER_SHORT = 1,
    VOSK_EP_ANSWER_LONG = 2,
    VOSK_EP_ANSWER_VERY_LONG = 3,
} VoskEndpointerMode;

/**
 * Set endpointer scaling factor
 *
 * @param mode - Endpointer mode
 **/
void vosk_recognizer_set_endpointer_mode(VoskRecognizer *recognizer,  VoskEndpointerMode mode);


/**
 * Set endpointer delays
 *
 * @param t_start_max     timeout for stopping recognition in case of initial silence (usually around 5.0)
 * @param t_end           timeout for stopping recognition in milliseconds after we recognized something (usually around 0.5 - 1.0)
 * @param t_max           timeout for forcing utterance end in milliseconds (usually around 20-30)
 **/
void vosk_recognizer_set_endpointer_delays(VoskRecognizer *recognizer, float t_start_max, float t_end, float t_max);


/** Releases recognizer object
 *
 *  Underlying model is also unreferenced and if needed released */
void vosk_recognizer_free(VoskRecognizer *recognizer);


/** Set log level for messages
 *
 *  @param log_level the level
 *     0 - default value to print info and error messages but no debug
 *     less than 0 - don't print info messages
 *     greater than 0 - more verbose mode
 */
void vosk_set_log_level(int log_level);


#ifdef __cplusplus
}
#endif

#endif /* VOSK_API_H */
