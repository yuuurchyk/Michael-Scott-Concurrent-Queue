# cpp_CourseProject

<style>
    ul{
        color: #ba184b;
    }
    p{
        color: black;
    }
    ul p, .sample p{
        margin: 0;
    }
    em{
        color: #ba184b;
        font-weight: 700;
    }
    ol li{
        padding-left: 15px;
    }
    .withoutDots{
        list-style-type: none;
    }
    #interface, #implementations, a, .path{
        color: #2980b9;
    }
    .codeCell{
        background-color: #F6F8FA;
        padding: 20px;
        border-radius: 3px;
        margin-left: 20px;
    }
    .codeCell p{
        margin: 0;
    }
    .success, .pcName{
        color: #27ae60;
    }
</style>

<h2>What is this?</h2>
<p>
    During this project I implemented a concurrent queue, which can be simultaneously accessed from both sides. I.e you can do <em>push</em> and <em>pop at the same time</em> and there are not lock with the same mutex.
</p>

<h2>References</h2>
<p>
    I implemented <em>Michael-Scott Algorithm</em>, which can be found <a href="http://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf">here</a>. The main challenge was <em>memory management</em>, which was not described in the article.
</p>

<h2>Interface and implementations</h2>
<p>Interface consist of several methods:</p>
<ul class="withoutDots" id="interface">
    <li>getPopMutex</li>
    <li>lock</li>
    <li>unlock</li>
    <li>push</li>
    <li>pop</li>
    <li>tryPop</li>
    <li>empty</li>
    <li>size</li>
</ul>
<p>And implementations:</p>
<ul class="withoutDots" id="implementations">
    <li>ConcurrentQueueSimple&lt;T&gt; (push and pop share the same mutex)</li>
    <li>ConcurrentQueueExtended&lt;T&gt; (What this project is all about)</li>
</ul>

<h2>Why there is no wait() and getConditionVariable()?</h2>
<p>As push and pop in ConcurrentQueueExtended affect different mutexes, you cannot simultaneously wait for both of them. Hence, there is no wait() method.</p>

<h2>Prerequisites</h2>
<p>
In order to build the project and run tests you should have <a href="https://cmake.org/">cmake</a> and <a href="https://github.com/google/googletest">gtest</a> installed.
</p>

<h2>How to build/run?</h2>

<p>Make Debug or Release directory</p>
<section class="codeCell">
    <p>mkdir Debug</p>
    <p>cd Debug</p>
</section>

<p>Let cmake generate make file and run it</p>
<section class="codeCell">
    <p>cmake -DCMAKE_BUILD_TYPE=Debug ..</p>
    <p>make</p>
</section>

<p>Run some of the tests</p>
<section class="codeCell">
    <p>./tests</p>
</section>

<h2>What tests are available?</h2>
<ul>
    <li>
        <p>
            <em>tests</em> - includes simple tests with integer queue, such as:
        </p>
        <ul>
            <li><p>initialization</p></li>
            <li><p>sequntial push</p></li>
            <li><p>sequential pop</p></li>
            <li><p>concurrent pop and push</p></li>
            <li><p>MPMC pattern</p></li>
        </ul>
        <p>the last 2 tests are the most important:</p>
        <dl>
            <dt><em>IncreaseToThousand</em></dt>
            <dd>
<p>There are numbers in increaseToThousandInitial withing small range ([0; 10]). We push them in the queue and perform the following operations in multiple threads:</p>
                <ul>
                    <li><p>get element from queue and increase in by 1</p></li>
                    <li><p>if it is >= 1000, push it in result queue</p></li>
                    <li><p>else push it back in the initial queue</p></li>
                </ul>
<p>We are using std::thread::hardware_concurrency() number of threads</p>
            </dd>
            <dt><em>AddNumbers</em></dt>
            <dd>
<p>Implementing <em>MPMC</em> pattern to compare implementations of concurrent queue.</p>
<p>There are producers: they push positive numbers in the queue.
There are consumers: they pop 2 elements from the queue and push their sum back.
The goal of the following process is to compute sum of all the numbers produced.
Consumers and producers work in paralel, so -1 is considered as a death pill.
Because we are using queue, not deque, I suggest the following algorithm for consumers.
We'll try to pop 3 elements from the queue.</p>
<p>Possible situations:</p>
                <ol>
                    <li><p>Nothing => we should wait</p></li>
                    <li><p>Death pill => consumer should push it back and die</p></li>
                    <li><p>Number => consumer should push it back</p></li>
                    <li><p>Number and death pill => consumer should push number and death pill back and die</p></li>
                    <li><p>2 numbers => consumer should add them, push sum back</p></li>
                    <li><p>3 numbers => same as in 5)</p></li>
                    <li><p>2 Numbers and death pill => add numbers, push sum back, push death pill back</p></li>
                </ol>
<p>We will be using half of available threads as producers and other half as consumers.</p>
            </dd>
        </dl>
    <p>You can run <em>tests</em> with 2 command line arguments:</p>
        <ul>
            <li><p>Number of samples in <em>IncreaseToThousand</em> (default: 100)</p></li>
            <li><p>Number of samples in <em>AddNumbers</em> (default: 100)</p></li>
        </ul>
    </li>
    <br/>
    <li>
        <p><em>stringTests</em> - includes similar tests with string queue, such as:</p>
        <ul>
            <li><p>initialization</p></li>
            <li><p>sequential push</p></li>
            <li><p>concurrent push</p></li>
            <li><p>MPMC pattern</p></li>
        </ul>
        <p>The last test is the most important:</p>
        <dl>
            <dt><em>ConcatenationMPMC</em></dt>
            <dd><p>The idea is pretty much the same as in <em>AddNumbers</em>, but now we are operating with strings</p></dd>
        </dl>
        <p>You can run <em>stringTests</em> with 1 command line argument:</p>
        <ul>
            <li><p>Number of samples in <em>ConcatenationMPMC</em> (default: 100)</p></li>
        </ul>
    </li>
</ul>

<h2>Examples run</h2>
<section class="codeCell">
    <p>
        <em>
            <span class="pcName">
                george@george-PC:
            </span>
            <span class="path">
                ~/Documents/cpp/cpp_CourseProject/Release
            </span>
        </em>
        $ ./stringTests 100000
    </p>
    <p>
    Testing:
    </p>
    <p>
        &nbsp;&nbsp;&nbsp;&nbsp;Samples num: 100000
    </p>
    <p>
        <span class="success">[==========]</span>
        Running 8 tests from 2 test cases.
    </p>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        Global test environment set-up.
    </p>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        4 tests from ConcurrentQueueInstantiation/ConcurrentQueueTest/0, where TypeParam = ConcurrentQueueSimple&lt;std::__cxx11::basic_string&lt;char, std::char_traits&lt;char&gt;, std::allocator&lt;char&gt; &gt; &gt;
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.Init
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.Init (0 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationSequential
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationSequential (530 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationMultiThreadProducer
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationMultiThreadProducer (431 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationMPMC
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/0.ConcatenationMPMC (3430 ms)
    </p>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        4 tests from ConcurrentQueueInstantiation/ConcurrentQueueTest/0 (4391 ms total)
    </p>
    <br/>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        4 tests from ConcurrentQueueInstantiation/ConcurrentQueueTest/1, where TypeParam = ConcurrentQueueExtended&lt;std::__cxx11::basic_string&lt;char, std::char_traits&lt;char&gt;, std::allocator&lt;char&gt; &gt; &gt;
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.Init
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.Init (4 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationSequential
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationSequential (471 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationMultiThreadProducer
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationMultiThreadProducer (439 ms)
    </p>
    <p>
        <span class="success">[&nbsp;RUN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationMPMC
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;OK&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        ConcurrentQueueInstantiation/ConcurrentQueueTest/1.ConcatenationMPMC (1247 ms)
    </p>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        4 tests from ConcurrentQueueInstantiation/ConcurrentQueueTest/1 (2161 ms total)
    </p>
    <br/>
    <p>
        <span class="success">[ - - - - - - - - - - ]</span>
        Global test environment tear-down
    </p>
    <p>
        <span class="success">[==========]</span>
        8 tests from 2 test cases ran. (6553 ms total)
    </p>
    <p>
        <span class="success">[&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PASSED&nbsp;&nbsp;&nbsp;&nbsp;]</span>
        8 tests from 2 test cases ran. (6553 ms total)
    </p>
</section>

<h2>Problems</h2>
<p>
Although <a href="http://valgrind.org/">valgrind</a> detects possible data race (which can bee seen <a href="cpp_CourseProject/valgrindLogDebug.txt">here</a>), I was unable to reproduce it. Hence, I created a simple <a href="cpp_CourseProject/testErrors.py">python script</a> that can help. It simply runs specified test program infinite number of times.
</p>
<p>Command line arguments:</p>
<ul>
    <li><p>Folder name (Debug or Release)</p></li>
    <li><p>Command line arguments for the program called</p></li>
</ul>
<p>Usage:</p>
<section class="codeCell">
    <p>
        <em>
            <span class="pcName">
                george@george-PC:
            </span>
            <span class="path">
                ~/Documents/cpp/cpp_CourseProject/
            </span>
        </em>
        $ python3 testErrors.py Debug tests 10 10
    </p>
</section>