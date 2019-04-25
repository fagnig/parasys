
import java.util.*;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.io.*;

/**
 * Search task.  No need to modify.
 */
class SearchTask implements Callable<List<Integer>>{

    char[] text, pattern;
    int from = 0, to = 0;       // Searched string: text[from..(to-1)]

    /**
     * Create a task for searching occurrences of 'pattern' in the substring text[from..(to-1)]
     *
     *   (to - from) must be at least the pattern length --- otherwise an index error will occur
     */
    public SearchTask(char[] text, char[] pattern, int from, int to) {
        this.text = text;  this.pattern = pattern;  this.from = from;  this.to = to;
    }

    public List<Integer> call() {
        final int pl = pattern.length;
        List<Integer> result = new LinkedList<Integer>();

        // VERY naive string matching to consume some CPU-cycles
        for (int i = from ; i <= to - pl; i++) {
            boolean eq = true;
            for (int j= 0; j < pl; j++) {
                if (text[i+j] != pattern[j]) eq = false;  // We really should break here
            }
            if (eq) result.add(i);
        } 

        return result;
    }
}


public class Search {

    static final int max = 10000000;        // Max no. of chars searched

    static char[]  text = new char[max];    // file to be searched
    static int     len;                     // Length of actual text
    static String  fname;                   // Text file name
    static char[]  pattern;                 // Search pattern
    static int     ntasks = 1;              // No. of tasks
    static int     nthreads = 1;            // No. of threads to use
    static boolean printPos = false;        // Print all positions found


    static void getArguments (String[] argv) {
        // Reads arguments into static variables
        try {
            int i = 0;

            while (i < argv.length) {

                /* Check for options*/
                if (argv[i].equals("-P")) {
                    printPos = true;
                    i++;
                    continue;
                }

                /* Handle positional parameters */
                fname = argv[i];
                pattern  = argv[i+1].toCharArray();
                i += 2;

                if (argv.length > i) {
                    ntasks = new Integer(argv[i]);
                    i++;
                }

                if (argv.length > i) {
                    nthreads = new Integer(argv[i]);
                    i++;
                }

                if (argv.length > i) 
                    throw new Exception("Too many arguments");
            }

            /* Read file into memory */
            InputStreamReader file = 
                    new InputStreamReader(new FileInputStream(fname));

            len = file.read(text);

            if (file.read() >=0) 
                System.out.println("\nWarning: file truncated to "+
                        max+" characters\n");

            if (ntasks <= 0 || nthreads <= 0 || pattern.length <= 0)
                                    throw new Exception("Illegal argument(s)");


        } catch (Exception e) {
            System.out.print(
                    e + 
                    "\n\nUsage:   java Search <options> file pattern [ntasks [nthreads]] \n\n" +
                    "  where: 0 < nthreads, 0 < ntasks, 0 < size(pattern)\n" +
                    "  Options: \n" +
                    "    -P           Print found positions\n"
                    );
            System.exit(1);
        }
    }

    static void writeResult(List<Integer> res, double time) {
        System.out.print("" + res.size() + " occurrences found");
        if (printPos) {
            int i = 0;
            System.out.println();
            for (int pos : res) {
                System.out.printf(" %6d", pos);
                if (++i % 10 == 0) System.out.println();
            }	    
            System.out.println();
        }
        System.out.printf(" in %1.6f s\n", time);
    }


    public static void main(String[] argv) {
        try {
	    long start;

            /* Get and print program parameters */
            getArguments(argv);
 	    System.out.printf("\nFile: %s, pattern: '%s', ntasks: %d, nthreads: %d\n",
                              fname, new String(pattern), ntasks, nthreads);

            /* Setup execution engine */
            //ExecutorService engine = Executors.newSingleThreadExecutor();
            //ExecutorService engine = Executors.newCachedThreadPool();
            ExecutorService engine = Executors.newFixedThreadPool(nthreads);

            /**********************************************
             * Run search using a single task
             *********************************************/
            SearchTask singleSearch = new SearchTask(text, pattern, 0, len);
	    double singleTime;
	    List<Integer> singleResult;
	    
            /* Run a couple of times on engine for loading all classes and warmup */
	    engine.submit(singleSearch).get();
	    engine.submit(singleSearch).get();

	    /* Run once more for time measurement and proper result */

	    start = System.nanoTime();
	    
	    singleResult = engine.submit(singleSearch).get();

	    singleTime = (double) (System.nanoTime()-start)/1e9;

 	    System.out.print("\nSingle task:    ");
            writeResult(singleResult, singleTime);

	    
            /**********************************************
             * Run search using multiple tasks
             *********************************************/

	    start = System.nanoTime();

	    /* Create list of tasks */
            List<SearchTask> taskList = new ArrayList<SearchTask>();
            // Add tasks to list here

            //ADDED

            int patternLen = new String(pattern).length(); //Length of pattern
            int bitesize = len/ntasks;  //amount of characters each task gets

            for(int j = 0; j < ntasks; j++){
                int startlocal = (bitesize*j) - ((patternLen-1)*java.lang.Math.min(j, 1));
                int endlocal = (bitesize*(j+1));

                if(j == ntasks-1){
                    endlocal = len;
                }

                taskList.add(new SearchTask(text,pattern,startlocal,endlocal));
            }


            /* Submit tasks  */
            List<Integer> completelist = new LinkedList<Integer>();
            List<Future<List<Integer>>> futureList = new LinkedList<Future<List<Integer>>>();
	    for (SearchTask task : taskList) {
		  Future<List<Integer>> future = engine.submit(task);
                // Collect futures here
            futureList.add(future);
	    }

        for (Future<List<Integer>> future : futureList){
            completelist.addAll(future.get());
        }


	    /* Overall result is an ordered list of unique occurrence positions */
 	    // Replace with a proper combination of task results!
            List<Integer> result = new LinkedList<Integer>();

            result.addAll(completelist);

            double time = (double) (System.nanoTime()-start)/1e9;

 	    System.out.printf("Using %2d tasks: ", ntasks);
            writeResult(result, time);

	    if (!singleResult.equals(result)) System.out.println("\nERROR: lists differ");
	    System.out.printf("\nSpeedup: %1.2f\n\n", singleTime/time);
	      

            /**********************************************
             * Terminate engine after use
             *********************************************/
            engine.shutdown();

        } catch (Exception e) { System.out.println("Search: "+e); }
    }
}






