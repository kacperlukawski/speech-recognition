#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <list>
#include <memory>
#include <vector>
#include <stdexcept>
#include "DataSample.h"

using std::list;
using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        /**
        * This abstract class is a base for all data sources used in the application.
        * By data source we define the source of the audio signal.
        *
        */
        template<typename FrameType>
        class DataSource {
        protected:
            std::shared_ptr<list<DataSample<FrameType>>> samples;

            /** Length of the single sample in milliseconds */
            int sampleLength;

            /**
             * Returns size of single sample with length 'sizeInMilliseconds'.
             */
            virtual unsigned int getDataSampleSize(int sizeInMilliseconds) = 0;

            /**
             * Returns length of sample with size 'size'.
             */
            virtual unsigned int getDataSampleLengthInMilliseconds(int size) = 0;

        public:

            template<typename dataSourceIteratorType>
            class dataSourceIterator {
                virtual bool operator==(const dataSourceIteratorType &it)=0;

                virtual bool operator!=(const dataSourceIteratorType &it)=0;

                virtual void operator++()=0;

                virtual DataSample<FrameType> operator*()=0;
            };

            class normalIterator : dataSourceIterator<normalIterator> {
            private:
                typename list<DataSample<FrameType>>::iterator samplesIterator;
            public:

                normalIterator(typename list<DataSample<FrameType>>::iterator samplesIterator) {
                    this->samplesIterator = samplesIterator;
                }

                virtual bool operator==(const normalIterator &it) {
                   return samplesIterator == it.samplesIterator;
                }

                virtual bool operator!=(const normalIterator &it) {
                    return samplesIterator != it.samplesIterator;
                }

                virtual void operator++() {
                    ++samplesIterator;
                }

                virtual DataSample<FrameType> operator*() {
                    return *samplesIterator;
                }
            };

            /**
             *
             */
            class offsetIterator : dataSourceIterator<offsetIterator>{
            private:
                friend class DataSource;
                DataSource<FrameType> &_dataSource;
                typename list<DataSample<FrameType>>::iterator samplesIterator;
                typename list<DataSample<FrameType>>::iterator samplesIteratorEnd;
                int offset;
                int dataSampleSize;
                int offsetInMilliseconds;
                int dataSampleSizeInMilliseconds;

                std::shared_ptr<FrameType> currIterationPtr;
                int currDataSampleSize;
                int currDataSamplePosition;

                std::vector<FrameType> *windowVector;

                offsetIterator(DataSource<FrameType> &dataSource, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds) : offsetIterator(dataSource, offsetInMilliseconds,
                                                                      dataSampleSizeInMilliseconds, false) {
                }

                offsetIterator(DataSource<FrameType> &dataSource, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds, bool endIterator) : _dataSource(dataSource) {
                    if (offsetInMilliseconds >= dataSampleSizeInMilliseconds) {
                        throw std::invalid_argument("Sample size must be bigger than offset size.");
                    }

                    this->offsetInMilliseconds = offsetInMilliseconds;
                    this->dataSampleSizeInMilliseconds = dataSampleSizeInMilliseconds;

                    this->offset = _dataSource.getDataSampleSize(offsetInMilliseconds);
                    this->dataSampleSize = _dataSource.getDataSampleSize(dataSampleSizeInMilliseconds);

                    this->windowVector = new std::vector<FrameType>();

                    if (endIterator) {
                        this->samplesIterator = this->samplesIteratorEnd = dataSource.samples->end();
                        samplesIterator--;
                        this->currDataSamplePosition = samplesIterator->getSize();
                        this->currDataSamplePosition += 2;
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        samplesIterator++;
                    } else {
                        this->samplesIterator = dataSource.samples->begin();
                        this->samplesIteratorEnd = --dataSource.samples->end();
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        this->currDataSamplePosition = 0;
                        ++(*this);
                    }
                }

            public:
                ~offsetIterator() {
                    windowVector->clear();
                    delete windowVector;
                    currIterationPtr.reset();
                }

                virtual bool operator==(const offsetIterator &it) {
                    return (this->currIterationPtr.get() == it.currIterationPtr.get() &&
                            this->currDataSamplePosition == it.currDataSamplePosition);
                }

                virtual bool operator!=(const offsetIterator &it) {
                    return !(*this == it);
                }

                // TODO: postfix operator++ is not defined

                virtual void operator++() {
                    std::vector<FrameType> vectorTmp;
                    if (windowVector->size() > offset) {
                        for (int i = windowVector->size() - offset; i < windowVector->size(); ++i) {
                            vectorTmp.push_back((*windowVector)[i]);
                        }
                    }

                    windowVector->clear();
                    windowVector->insert(windowVector->begin(), vectorTmp.begin(), vectorTmp.end());

                    while (windowVector->size() < dataSampleSize) {
                        for (; currDataSamplePosition < currDataSampleSize &&
                               windowVector->size() < dataSampleSize; ++currDataSamplePosition) {
                            windowVector->push_back(currIterationPtr.get()[currDataSamplePosition]);
                        }
                        if (currDataSamplePosition >= currDataSampleSize) {
                            if (samplesIterator != samplesIteratorEnd) {
                                ++samplesIterator;
                                currIterationPtr = samplesIterator->getValues();
                                currDataSampleSize = samplesIterator->getSize();
                                currDataSamplePosition = 0;
                            } else {
                                ++currDataSamplePosition;
                                return;
                            }
                        }
                    }
                }

                virtual DataSample<FrameType> operator*() {
                    int windowSize = (*windowVector).size();
                    std::shared_ptr<FrameType> windowVectorPtr(new FrameType[windowSize],
                                                               std::default_delete<FrameType[]>());
                    for (int i = 0; i < windowSize; ++i) {
                        windowVectorPtr.get()[i] = (*windowVector)[i];
                    }
                    auto size = _dataSource.getDataSampleLengthInMilliseconds(windowSize);
                    return DataSample<FrameType>(windowSize, size, windowVectorPtr);
                }
            };

            /**
             * The default constructor of DataSource is here just to have
             * a possibility to declare arrays of such objects
             */
            DataSource() : DataSource(0) {}

            DataSource(int sampleLength);

            virtual ~DataSource();

            virtual void init();

            virtual void addSample(DataSample<FrameType> sample);

            virtual DataSource<FrameType>::normalIterator getSamplesIteratorBegin();

            virtual DataSource<FrameType>::normalIterator getSamplesIteratorEnd();

            virtual DataSource<FrameType>::offsetIterator getOffsetIteratorBegin(int windowSizeInMilliseconds,
                                                                           int offsetInMilliseconds);

            virtual DataSource<FrameType>::offsetIterator getOffsetIteratorEnd();
        };

    }

}

#endif