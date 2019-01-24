/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AAPT_TEST_FIXTURE_H
#define AAPT_TEST_FIXTURE_H

#include "android-base/file.h"
#include "android-base/macros.h"
#include "androidfw/StringPiece.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "io/Util.h"
#include "util/Files.h"
#include "LoadedApk.h"

namespace aapt {

class TestDirectoryFixture : public ::testing::Test {
 public:
  TestDirectoryFixture() = default;
  virtual ~TestDirectoryFixture() = default;

  // Creates the test directory or clears its contents if it contains previously created files.
  void SetUp() override;

  // Clears the contents of the test directory.
  void TearDown() override;

  // Retrieve the test directory of the fixture.
  const android::StringPiece GetTestDirectory() {
    return temp_dir_;
  }

  // Retrieves the absolute path of the specified relative path in the test directory. Directories
  // should be separated using forward slashes ('/'), and these slashes will be translated to
  // backslashes when running Windows tests.
  const std::string GetTestPath(const android::StringPiece& path) {
    std::string base = temp_dir_;
    for (android::StringPiece part : util::Split(path, '/')) {
      file::AppendPath(&base, part);
    }
    return base;
  }

  // Creates a file with the specified contents, creates any intermediate directories in the
  // process. The file path must be an absolute path within the test directory.
  bool WriteFile(const std::string& path, const std::string& contents);

 private:
  std::string temp_dir_;
  DISALLOW_COPY_AND_ASSIGN(TestDirectoryFixture);
};

class CommandTestFixture : public TestDirectoryFixture {
 public:
  CommandTestFixture() = default;
  virtual ~CommandTestFixture() = default;

  // Wries the contents of the file to the specified path. The file is compiled and the flattened
  // file is written to the out directory.
  bool CompileFile(const std::string& path, const std::string& contents,
                   const android::StringPiece& flat_out_dir, IDiagnostics* diag);

  // Executes the link command with the specified arguments. The flattened files residing in the
  // flat directory will be added to the link command as file arguments.
  bool Link(const std::vector<std::string>& args, const android::StringPiece& flat_dir,
            IDiagnostics* diag);

  // Creates a minimal android manifest within the test directory and returns the file path.
  std::string GetDefaultManifest();

  // Returns pointer to data inside APK files
  std::unique_ptr<io::IData> OpenFileAsData(LoadedApk* apk,
                                            const android::StringPiece& path);

  // Asserts that loading the tree from the specified file in the apk succeeds.
  void AssertLoadXml(LoadedApk* apk, const io::IData* data,
                     android::ResXMLTree* out_tree);

 private:
  DISALLOW_COPY_AND_ASSIGN(CommandTestFixture);
};

} // namespace aapt

#endif  // AAPT_TEST_FIXTURE_H