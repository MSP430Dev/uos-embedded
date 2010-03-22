MODULES		= timer

DOCBOOK		= about_ru.xml \
		  license_ru.xml \
		  kernel_ru.xml \
		  kernel_start_ru.xml \
		  kernel_tasks_ru.xml \
		  kernel_mutexes_ru.xml \
		  kernel_irqs_ru.xml \
		  compile_target_ru.xml \
		  compile_module_ru.xml

DOXYFLAGS	= echo "STRIP_FROM_PATH = ../trunk/sources/";\
		  echo "EXCLUDE_PATTERNS += .svn";\
		  echo "PROJECT_NAME = uOS";\
		  echo "OUTPUT_DIRECTORY = .";\
		  echo "GENERATE_XML = YES";\
		  echo "GENERATE_HTML = NO";\
		  echo "GENERATE_LATEX = NO";\
		  echo "XML_OUTPUT = xml";\
		  echo "HIDE_UNDOC_MEMBERS = YES";\
		  echo "HIDE_UNDOC_CLASSES = YES";\
		  echo "QUIET = YES";\
		  echo "WARNINGS = YES";\
		  echo "ENABLE_PREPROCESSING = YES";

all:
		@for m in $(MODULES); do\
			[ -f ../trunk/sources/$$m/doxygen.cfg ] || continue;\
			echo Processing $$m;\
			rm -rf xml;\
			( cat ../trunk/sources/$$m/doxygen.cfg;\
			echo "INPUT = ../trunk/sources/$$m";\
			echo "OUTPUT_LANGUAGE = English";\
			$(DOXYFLAGS) ) | doxygen -;\
			python doxy2wiki.py -v $$m > en_$$m.wiki;\
			rm -rf xml;\
			( cat ../trunk/sources/$$m/doxygen.cfg;\
			echo "INPUT = ../trunk/sources/$$m";\
			echo "OUTPUT_LANGUAGE = Russian";\
			$(DOXYFLAGS) ) | doxygen -;\
			python doxy2wiki.py -v -lru $$m > ru_$$m.wiki;\
		done

clean:
		rm -rf *~ xml *.xml *.rtf

docbook:	$(DOCBOOK)

verify:		$(DOCBOOK)
		for doc in $(DOCBOOK); do echo $$doc:; xmllint --noout --valid $$doc; done

rtf:		$(DOCBOOK:%.xml=%.rtf)
		#for doc in $(DOCBOOK); do echo $$doc:; docbook2rtf $$doc; done

.SUFFIXES:	.wiki .xml .rtf

.wiki.xml:;	./gwiki2docbook.py -l ru $< > $@
.xml.rtf:;	docbook2rtf $<
